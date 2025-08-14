#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>
#include <cctype>

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
            // 判断是局部样式块还是全局样式块
            // 局部样式块：在元素内部（状态栈中有元素状态）
            // 全局样式块：在顶层
            bool isLocalStyle = !stateStack_.empty();
            
            if (isLocalStyle) {
                // 局部样式块整体归属于 CHTL
                // 继续在 CHTL 片段中
                // 扫描整个局部样式块
                appendToFragment('s');
                appendToFragment('t');
                appendToFragment('y');
                appendToFragment('l');
                appendToFragment('e');
                for (int i = 0; i < 5; i++) advance(); // 移动位置
                
                skipWhitespace();
                appendToFragment(' ');
                appendToFragment(advance()); // '{'
                
                int braceCount = 1;
                while (!isAtEnd() && braceCount > 0) {
                    char ch = peek();
                    appendToFragment(advance());
                    if (ch == '{') braceCount++;
                    else if (ch == '}') braceCount--;
                }
            } else {
                // 全局样式块
                // style { 归属于 CHTL
                appendToFragment('s');
                appendToFragment('t');
                appendToFragment('y');
                appendToFragment('l');
                appendToFragment('e');
                for (int i = 0; i < 5; i++) advance(); // 移动位置
                
                skipWhitespace();
                
                appendToFragment(advance()); // '{'
                
                // 结束 CHTL 片段
                endFragment();
                
                // 开始 CSS 片段
                startFragment(FragmentType::CSS);
                pushState(ScannerState::IN_STYLE);
            }
            return;
        }
        
        // 检查 script 块
        if (matchKeyword("script") && peek() == '{') {
            // script { 归属于 CHTL
            appendToFragment('s');
            appendToFragment('c');
            appendToFragment('r');
            appendToFragment('i');
            appendToFragment('p');
            appendToFragment('t');
            for (int i = 0; i < 6; i++) advance(); // 移动位置
            
            skipWhitespace();
            
            appendToFragment(advance()); // '{'
            
            // 结束 CHTL 片段
            endFragment();
            
            // 开始 JavaScript 片段（默认）
            startFragment(FragmentType::JAVASCRIPT);
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
        // 检查 CHTL 特性（如 @Var 等）
        if (peek() == '@' && std::isalpha(peek(1))) {
            // 结束当前 CSS 片段
            endFragment();
            // 开始 CHTL 片段
            startFragment(FragmentType::CHTL);
            
            // 扫描 CHTL 特性
            appendToFragment(advance()); // @
            scanIdentifier(); // Var, Element等
            
            // 如果有参数
            if (peek() == '(') {
                scanBalancedParentheses();
            }
            
            // 结束 CHTL 片段
            endFragment();
            // 继续 CSS 片段
            startFragment(FragmentType::CSS);
            continue;
        }
        
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
                // 结束 CSS 片段
                endFragment();
                // style 的结束 } 归属于 CHTL
                startFragment(FragmentType::CHTL);
                popState();
                return;
            }
        }
    }
}

void CHTLUnifiedScanner::scanScript() {
    int braceCount = 1;
    bool inJavaScript = true; // 默认为 JavaScript
    
    while (!isAtEnd() && braceCount > 0) {
        // 检查是否需要切换到 CHTL JS
        if (inJavaScript && matchSequence("{{")) {
            // 结束当前 JavaScript 片段
            endFragment();
            // 开始 CHTL JS 片段
            startFragment(FragmentType::CHTL_JS);
            inJavaScript = false;
            scanCHTLJS();
            // 结束 CHTL JS 片段
            endFragment();
            // 开始新的 JavaScript 片段
            startFragment(FragmentType::JAVASCRIPT);
            inJavaScript = true;
            continue;
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
                // 结束脚本片段
                endFragment();
                // script 的结束 } 归属于 CHTL
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

bool CHTLUnifiedScanner::isCHTLJSContinuation() {
    // 跳过空白
    size_t pos = currentPos_;
    while (pos < source_.length() && std::isspace(source_[pos])) {
        pos++;
    }
    
    if (pos >= source_.length()) {
        return false;
    }
    
    // 检查 -> 操作符
    if (pos + 1 < source_.length() && source_[pos] == '-' && source_[pos + 1] == '>') {
        return true;
    }
    
    // 检查 . 后跟 CHTL JS 方法
    if (source_[pos] == '.') {
        pos++;
        std::string method = peekIdentifierAt(pos);
        return isCHTLJSMethod(method);
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCHTLJSMethod(const std::string& method) {
    // 根据语法文档，CHTL JS 的专有方法
    static const std::set<std::string> chtlJSMethods = {
        "listen",     // 事件监听
        "delegate",   // 事件委托
        "animate"     // 动画
    };
    
    return chtlJSMethods.count(method) > 0;
}

std::string CHTLUnifiedScanner::peekIdentifier() {
    size_t pos = currentPos_;
    return peekIdentifierAt(pos);
}

std::string CHTLUnifiedScanner::peekIdentifierAt(size_t pos) {
    std::string identifier;
    
    if (pos < source_.length() && (std::isalpha(source_[pos]) || source_[pos] == '_')) {
        identifier += source_[pos++];
        
        while (pos < source_.length() && 
               (std::isalnum(source_[pos]) || source_[pos] == '_')) {
            identifier += source_[pos++];
        }
    }
    
    return identifier;
}

void CHTLUnifiedScanner::scanIdentifier() {
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        appendToFragment(advance());
    }
}

void CHTLUnifiedScanner::scanBalancedParentheses() {
    if (peek() != '(') return;
    
    appendToFragment(advance()); // '('
    int count = 1;
    
    while (!isAtEnd() && count > 0) {
        if (peek() == '(') {
            count++;
        } else if (peek() == ')') {
            count--;
        } else if (peek() == '"' || peek() == '\'') {
            char quote = peek();
            appendToFragment(advance());
            scanString(quote);
            continue;
        }
        appendToFragment(advance());
    }
}

void CHTLUnifiedScanner::scanCHTLJS() {
    // 扫描 {{selector}}
    appendToFragment(advance()); // {
    appendToFragment(advance()); // {
    
    while (!isAtEnd() && !(peek() == '}' && peek(1) == '}')) {
        appendToFragment(advance());
    }
    
    if (!isAtEnd()) {
        appendToFragment(advance()); // }
        appendToFragment(advance()); // }
    }
    
    // 继续扫描 CHTL JS 的延续部分
    while (isCHTLJSContinuation()) {
        skipWhitespace();
        
        if (peek() == '-' && peek(1) == '>') {
            appendToFragment(advance()); // -
            appendToFragment(advance()); // >
        } else if (peek() == '.') {
            appendToFragment(advance()); // .
            // 扫描方法名
            scanIdentifier();
            // 扫描方法参数
            if (peek() == '(') {
                scanBalancedParentheses();
            }
        }
    }
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