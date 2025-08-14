#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace chtl {
namespace scanner {

// CodeFragment 实现
std::string CodeFragment::toString() const {
    std::stringstream ss;
    ss << "Fragment[";
    switch (type) {
        case FragmentType::CHTL: ss << "CHTL"; break;
        case FragmentType::CHTL_JS: ss << "CHTL_JS"; break;
        case FragmentType::CSS: ss << "CSS"; break;
        case FragmentType::JAVASCRIPT: ss << "JS"; break;
    }
    ss << ", " << startLine << ":" << startColumn 
       << " - " << endLine << ":" << endColumn
       << ", length=" << content.length() << "]";
    return ss.str();
}

// CHTLUnifiedScanner 实现
CHTLUnifiedScanner::CHTLUnifiedScanner()
    : currentState_(ScannerState::CHTL)
    , currentLine_(1)
    , currentColumn_(1)
    , currentPos_(0)
    , debugMode_(false) {
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan(const std::string& source) {
    // 初始化
    source_ = source;
    fragments_.clear();
    currentPos_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
    currentState_ = ScannerState::CHTL;
    stateStack_.clear();
    currentBuffer_.clear();
    
    // 开始扫描
    startFragment(FragmentType::CHTL);
    
    while (!isAtEnd()) {
        switch (currentState_) {
            case ScannerState::CHTL:
                scanCHTL();
                break;
            case ScannerState::IN_STYLE:
                scanStyle();
                break;
            case ScannerState::IN_SCRIPT:
                scanScript();
                break;
            case ScannerState::IN_STRING:
                // 字符串扫描由其他状态处理
                break;
            case ScannerState::IN_COMMENT:
                scanComment();
                break;
            case ScannerState::IN_ORIGIN:
                scanOrigin();
                break;
        }
    }
    
    // 结束当前片段
    if (!currentBuffer_.empty()) {
        endFragment();
    }
    
    if (debugMode_) {
        std::cout << "Scanner found " << fragments_.size() << " fragments:\n";
        for (const auto& frag : fragments_) {
            std::cout << "  " << frag.toString() << "\n";
        }
    }
    
    return fragments_;
}

void CHTLUnifiedScanner::scanCHTL() {
    while (!isAtEnd()) {
        // 检查注释
        if (matchSequence("//")) {
            pushState(ScannerState::IN_COMMENT);
            return;
        }
        if (matchSequence("/*")) {
            pushState(ScannerState::IN_COMMENT);
            return;
        }
        
        // 检查字符串
        if (peek() == '"' || peek() == '\'') {
            char quote = advance();
            scanString(quote);
            continue;
        }
        
        // 检查 style 块
        if (matchKeyword("style") && peek() == '{') {
            // 结束当前 CHTL 片段
            endFragment();
            
            // 跳过 style 和 {
            for (int i = 0; i < 5; i++) advance(); // "style"
            skipWhitespace();
            advance(); // '{'
            
            // 开始 CSS 片段
            startFragment(FragmentType::CSS);
            pushState(ScannerState::IN_STYLE);
            return;
        }
        
        // 检查 script 块
        if (matchKeyword("script") && peek() == '{') {
            // 结束当前 CHTL 片段
            endFragment();
            
            // 跳过 script 和 {
            for (int i = 0; i < 6; i++) advance(); // "script"
            skipWhitespace();
            advance(); // '{'
            
            // 开始脚本片段
            startFragment(FragmentType::CHTL_JS);
            pushState(ScannerState::IN_SCRIPT);
            return;
        }
        
        // 检查 [Origin] 块
        if (matchSequence("[Origin]")) {
            pushState(ScannerState::IN_ORIGIN);
            return;
        }
        
        // 普通字符
        appendToFragment(advance());
    }
}

void CHTLUnifiedScanner::scanStyle() {
    int braceCount = 1;
    
    while (!isAtEnd() && braceCount > 0) {
        // 检查注释
        if (matchSequence("/*")) {
            appendToFragment(advance());
            appendToFragment(advance());
            while (!isAtEnd() && !matchSequence("*/")) {
                appendToFragment(advance());
            }
            if (matchSequence("*/")) {
                appendToFragment(advance());
                appendToFragment(advance());
            }
            continue;
        }
        
        // 检查字符串
        if (peek() == '"' || peek() == '\'') {
            char quote = advance();
            appendToFragment(quote);
            scanString(quote);
            continue;
        }
        
        // 计算花括号
        char ch = advance();
        appendToFragment(ch);
        
        if (ch == '{') {
            braceCount++;
        } else if (ch == '}') {
            braceCount--;
            if (braceCount == 0) {
                // 结束 CSS 片段，返回 CHTL 状态
                endFragment();
                startFragment(FragmentType::CHTL);
                popState();
                return;
            }
        }
    }
}

void CHTLUnifiedScanner::scanScript() {
    int braceCount = 1;
    
    while (!isAtEnd() && braceCount > 0) {
        // 检查 CHTL JS 特性
        if (isCHTLJSFeature()) {
            // 保持在 CHTL_JS 片段中
        }
        
        // 检查注释
        if (matchSequence("//")) {
            appendToFragment(advance());
            appendToFragment(advance());
            while (!isAtEnd() && peek() != '\n') {
                appendToFragment(advance());
            }
            continue;
        }
        
        if (matchSequence("/*")) {
            appendToFragment(advance());
            appendToFragment(advance());
            while (!isAtEnd() && !matchSequence("*/")) {
                appendToFragment(advance());
            }
            if (matchSequence("*/")) {
                appendToFragment(advance());
                appendToFragment(advance());
            }
            continue;
        }
        
        // 检查字符串
        if (peek() == '"' || peek() == '\'') {
            char quote = advance();
            appendToFragment(quote);
            scanString(quote);
            continue;
        }
        
        // 计算花括号
        char ch = advance();
        appendToFragment(ch);
        
        if (ch == '{') {
            braceCount++;
        } else if (ch == '}') {
            braceCount--;
            if (braceCount == 0) {
                // 结束脚本片段，返回 CHTL 状态
                endFragment();
                startFragment(FragmentType::CHTL);
                popState();
                return;
            }
        }
    }
}

void CHTLUnifiedScanner::scanString(char quote) {
    // 字符串内容保持在当前片段
    while (!isAtEnd()) {
        char ch = peek();
        if (ch == quote) {
            appendToFragment(advance());
            break;
        } else if (ch == '\\') {
            appendToFragment(advance()); // '\'
            if (!isAtEnd()) {
                appendToFragment(advance()); // 转义字符
            }
        } else {
            appendToFragment(advance());
        }
    }
}

void CHTLUnifiedScanner::scanComment() {
    if (currentPos_ >= 2 && source_[currentPos_ - 2] == '/' && source_[currentPos_ - 1] == '/') {
        // 单行注释
        while (!isAtEnd() && peek() != '\n') {
            appendToFragment(advance());
        }
        popState();
    } else if (currentPos_ >= 2 && source_[currentPos_ - 2] == '/' && source_[currentPos_ - 1] == '*') {
        // 多行注释
        while (!isAtEnd()) {
            if (matchSequence("*/")) {
                appendToFragment(advance());
                appendToFragment(advance());
                popState();
                break;
            }
            appendToFragment(advance());
        }
    }
}

void CHTLUnifiedScanner::scanOrigin() {
    // 扫描 [Origin] @Type { ... }
    while (!isAtEnd()) {
        if (peek() == '{') {
            advance();
            int braceCount = 1;
            
            while (!isAtEnd() && braceCount > 0) {
                char ch = advance();
                appendToFragment(ch);
                
                if (ch == '{') {
                    braceCount++;
                } else if (ch == '}') {
                    braceCount--;
                    if (braceCount == 0) {
                        popState();
                        return;
                    }
                }
            }
        } else {
            appendToFragment(advance());
        }
    }
}

bool CHTLUnifiedScanner::isCHTLJSFeature() {
    // 检测 {{}} 选择器
    if (matchSequence("{{")) {
        return true;
    }
    
    // 检测 -> 链式访问
    if (matchSequence("->")) {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::isInCHTLJSContext() {
    return currentState_ == ScannerState::IN_SCRIPT;
}

// 辅助方法实现
void CHTLUnifiedScanner::pushState(ScannerState state) {
    stateStack_.push_back(currentState_);
    currentState_ = state;
}

void CHTLUnifiedScanner::popState() {
    if (!stateStack_.empty()) {
        currentState_ = stateStack_.back();
        stateStack_.pop_back();
    }
}

void CHTLUnifiedScanner::startFragment(FragmentType type) {
    currentBuffer_.clear();
    fragmentStartLine_ = currentLine_;
    fragmentStartColumn_ = currentColumn_;
}

void CHTLUnifiedScanner::endFragment() {
    if (!currentBuffer_.empty()) {
        fragments_.push_back({
            (currentState_ == ScannerState::IN_STYLE) ? FragmentType::CSS :
            (currentState_ == ScannerState::IN_SCRIPT) ? FragmentType::CHTL_JS :
            FragmentType::CHTL,
            currentBuffer_,
            fragmentStartLine_,
            fragmentStartColumn_,
            currentLine_,
            currentColumn_
        });
        currentBuffer_.clear();
    }
}

void CHTLUnifiedScanner::appendToFragment(char ch) {
    currentBuffer_ += ch;
}

void CHTLUnifiedScanner::appendToFragment(const std::string& str) {
    currentBuffer_ += str;
}

bool CHTLUnifiedScanner::matchKeyword(const std::string& keyword) {
    if (currentPos_ + keyword.length() > source_.length()) {
        return false;
    }
    
    // 检查关键字匹配
    for (size_t i = 0; i < keyword.length(); i++) {
        if (source_[currentPos_ + i] != keyword[i]) {
            return false;
        }
    }
    
    // 检查后面是否是非字母数字字符（确保是完整单词）
    if (currentPos_ + keyword.length() < source_.length()) {
        char nextChar = source_[currentPos_ + keyword.length()];
        if (std::isalnum(nextChar) || nextChar == '_') {
            return false;
        }
    }
    
    return true;
}

bool CHTLUnifiedScanner::matchSequence(const std::string& seq) {
    if (currentPos_ + seq.length() > source_.length()) {
        return false;
    }
    
    for (size_t i = 0; i < seq.length(); i++) {
        if (source_[currentPos_ + i] != seq[i]) {
            return false;
        }
    }
    
    return true;
}

bool CHTLUnifiedScanner::isAtEnd() const {
    return currentPos_ >= source_.length();
}

char CHTLUnifiedScanner::peek(int offset) const {
    if (currentPos_ + offset >= source_.length()) {
        return '\0';
    }
    return source_[currentPos_ + offset];
}

char CHTLUnifiedScanner::advance() {
    if (isAtEnd()) {
        return '\0';
    }
    char ch = source_[currentPos_++];
    updatePosition(ch);
    return ch;
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd() && std::isspace(peek())) {
        advance();
    }
}

void CHTLUnifiedScanner::updatePosition(char ch) {
    if (ch == '\n') {
        currentLine_++;
        currentColumn_ = 1;
    } else {
        currentColumn_++;
    }
}

} // namespace scanner
} // namespace chtl