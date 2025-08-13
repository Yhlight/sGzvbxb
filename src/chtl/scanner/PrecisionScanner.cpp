#include "PrecisionScanner.h"
#include <cctype>
#include <algorithm>

namespace chtl {
namespace scanner {

PrecisionScanner::PrecisionScanner() : CHTLUnifiedScanner() {}

std::vector<CodeFragment> PrecisionScanner::scan(const std::string& input) {
    input_ = input;
    pos_ = 0;
    line_ = 1;
    column_ = 1;
    errors_.clear();
    
    std::vector<CodeFragment> fragments;
    
    // 初始化上下文
    pushContext(ScanMode::CHTL);
    
    // 开始精确扫描
    scanPrecise(fragments);
    
    return fragments;
}

void PrecisionScanner::scanPrecise(std::vector<CodeFragment>& fragments) {
    while (pos_ < input_.size()) {
        skipWhitespace();
        if (pos_ >= input_.size()) break;
        
        ScanMode mode = currentMode();
        
        switch (mode) {
            case ScanMode::CHTL:
                // 在CHTL模式下，严格扫描每个特征
                if (!scanCHTLFeature(fragments)) {
                    // 如果不是CHTL特征，作为普通CHTL内容
                    size_t start = pos_;
                    size_t startLine = line_;
                    size_t startCol = column_;
                    
                    // 扫描到下一个可能的特征或块边界
                    std::string content;
                    while (pos_ < input_.size()) {
                        if (peek() == '@' || peek() == '[' || 
                            (peek() == 's' && matchExact("style")) ||
                            (peek() == 's' && matchExact("script")) ||
                            (peek() == 't' && matchExact("text"))) {
                            break;
                        }
                        content += advance();
                    }
                    
                    if (!content.empty()) {
                        addFragment(fragments, FragmentType::CHTL, content,
                                   startLine, startCol, line_, column_);
                    }
                }
                break;
                
            case ScanMode::SCRIPT_BLOCK:
                // 在script块内，检查CHTL JS特征
                if (!scanCHTLJSFeature(fragments)) {
                    // 不是CHTL JS特征，作为普通JS
                    scanJSBlock(fragments);
                }
                break;
                
            case ScanMode::STYLE_BLOCK:
                // 在style块内，根据是否全局决定处理方式
                if (contextStack_.top().isGlobalStyle) {
                    scanCSSBlock(fragments);
                } else {
                    // 局部style由CHTL处理
                    if (!scanCHTLFeature(fragments)) {
                        advance();  // 跳过单个字符
                    }
                }
                break;
                
            case ScanMode::CSS:
                scanCSSBlock(fragments);
                break;
                
            case ScanMode::JS:
                scanJSBlock(fragments);
                break;
                
            case ScanMode::CHTL_JS:
                if (!scanCHTLJSFeature(fragments)) {
                    advance();
                }
                break;
        }
    }
}

bool PrecisionScanner::scanCHTLFeature(std::vector<CodeFragment>& fragments) {
    // 扫描 @Element, @Style, @Var
    if (scanAtDirective(fragments)) return true;
    
    // 扫描 [Template], [Custom], [Import] 等
    if (scanTemplateDirective(fragments)) return true;
    if (scanCustomDirective(fragments)) return true;
    
    // 扫描 text { }
    if (scanTextBlock(fragments)) return true;
    
    // 扫描 style { } 或 script { }
    if (peek() == 's') {
        if (matchExact("style")) {
            size_t start = pos_;
            size_t startLine = line_;
            size_t startCol = column_;
            
            pos_ += 5;  // "style"
            column_ += 5;
            skipWhitespace();
            
            if (peek() == '{') {
                // 判断是否是全局style
                bool isGlobal = !contextStack_.top().inElement;
                
                std::string styleKeyword = "style";
                addFragment(fragments, FragmentType::CHTL, styleKeyword,
                           startLine, startCol, line_, column_);
                
                advance(); // '{'
                pushContext(ScanMode::STYLE_BLOCK);
                contextStack_.top().isGlobalStyle = isGlobal;
                
                return true;
            }
        } else if (matchExact("script")) {
            size_t start = pos_;
            size_t startLine = line_;
            size_t startCol = column_;
            
            pos_ += 6;  // "script"
            column_ += 6;
            skipWhitespace();
            
            if (peek() == '{') {
                std::string scriptKeyword = "script";
                addFragment(fragments, FragmentType::CHTL, scriptKeyword,
                           startLine, startCol, line_, column_);
                
                advance(); // '{'
                pushContext(ScanMode::SCRIPT_BLOCK);
                
                return true;
            }
        }
    }
    
    return false;
}

bool PrecisionScanner::scanAtDirective(std::vector<CodeFragment>& fragments) {
    if (peek() != '@') return false;
    
    size_t start = pos_;
    size_t startLine = line_;
    size_t startCol = column_;
    
    advance(); // '@'
    
    // 扫描标识符
    std::string directive = scanIdentifier();
    
    if (directive == "Element" || directive == "Style" || directive == "Var") {
        // 这是CHTL特征，精确截断
        std::string content = "@" + directive;
        
        // 如果后面有参数，继续扫描
        skipWhitespace();
        if (peek() == '(') {
            content += advance(); // '('
            
            // 扫描到匹配的 ')'
            int parenCount = 1;
            while (pos_ < input_.size() && parenCount > 0) {
                char c = peek();
                if (c == '(') parenCount++;
                else if (c == ')') parenCount--;
                content += advance();
                if (parenCount == 0) break;
            }
        }
        
        addFragment(fragments, FragmentType::CHTL, content,
                   startLine, startCol, line_, column_);
        return true;
    }
    
    // 不是CHTL特征，回退
    pos_ = start;
    line_ = startLine;
    column_ = startCol;
    return false;
}

bool PrecisionScanner::scanCHTLJSFeature(std::vector<CodeFragment>& fragments) {
    // 扫描 {{ }}
    if (scanInterpolation(fragments)) return true;
    
    // 扫描 ->
    if (scanChainOperator(fragments)) return true;
    
    // 扫描 .listen(
    if (scanListenMethod(fragments)) return true;
    
    // 扫描 animate(
    if (scanAnimateFunction(fragments)) return true;
    
    return false;
}

bool PrecisionScanner::scanInterpolation(std::vector<CodeFragment>& fragments) {
    if (peek() != '{' || peekNext() != '{') return false;
    
    size_t start = pos_;
    size_t startLine = line_;
    size_t startCol = column_;
    
    std::string content = "{{";
    advance(); advance(); // '{{'
    
    // 扫描到 }}
    while (pos_ < input_.size()) {
        if (peek() == '}' && peekNext() == '}') {
            content += "}}";
            advance(); advance();
            break;
        }
        content += advance();
    }
    
    addFragment(fragments, FragmentType::CHTL_JS, content,
               startLine, startCol, line_, column_);
    return true;
}

bool PrecisionScanner::scanChainOperator(std::vector<CodeFragment>& fragments) {
    if (peek() != '-' || peekNext() != '>') return false;
    
    size_t start = pos_;
    size_t startLine = line_;
    size_t startCol = column_;
    
    advance(); advance(); // '->'
    
    addFragment(fragments, FragmentType::CHTL_JS, "->",
               startLine, startCol, line_, column_);
    return true;
}

bool PrecisionScanner::scanListenMethod(std::vector<CodeFragment>& fragments) {
    size_t savePos = pos_;
    size_t saveLine = line_;
    size_t saveCol = column_;
    
    // 检查 .listen( 或空格后的 listen(
    bool hasDot = false;
    if (peek() == '.') {
        advance();
        hasDot = true;
    }
    
    if (matchExact("listen") && peek() == '(') {
        size_t start = hasDot ? savePos : pos_ - 6;  // "listen"的长度
        
        std::string content = hasDot ? ".listen(" : "listen(";
        pos_ += 6; // "listen"
        advance(); // '('
        
        // 扫描整个listen调用
        int parenCount = 1;
        while (pos_ < input_.size() && parenCount > 0) {
            char c = peek();
            if (c == '(') parenCount++;
            else if (c == ')') parenCount--;
            content += advance();
            if (parenCount == 0) break;
        }
        
        addFragment(fragments, FragmentType::CHTL_JS, content,
                   saveLine, saveCol, line_, column_);
        return true;
    }
    
    // 回退
    pos_ = savePos;
    line_ = saveLine;
    column_ = saveCol;
    return false;
}

bool PrecisionScanner::scanAnimateFunction(std::vector<CodeFragment>& fragments) {
    if (!matchExact("animate") || peek() != '(') return false;
    
    size_t start = pos_ - 7;  // "animate"的长度
    size_t startLine = line_;
    size_t startCol = column_ - 7;
    
    std::string content = "animate(";
    advance(); // '('
    
    // 扫描整个animate调用
    int parenCount = 1;
    while (pos_ < input_.size() && parenCount > 0) {
        char c = peek();
        if (c == '(') parenCount++;
        else if (c == ')') parenCount--;
        content += advance();
        if (parenCount == 0) break;
    }
    
    addFragment(fragments, FragmentType::CHTL_JS, content,
               startLine, startCol, line_, column_);
    return true;
}

void PrecisionScanner::scanCSSBlock(std::vector<CodeFragment>& fragments) {
    // CSS采用宽松的块级别处理
    size_t start = pos_;
    size_t startLine = line_;
    size_t startCol = column_;
    
    std::string content = scanBalancedBraces();
    
    if (!content.empty()) {
        addFragment(fragments, FragmentType::CSS, content,
                   startLine, startCol, line_, column_);
    }
}

void PrecisionScanner::scanJSBlock(std::vector<CodeFragment>& fragments) {
    // JS采用宽松的块级别处理，但要检查@Var等CHTL特征
    size_t start = pos_;
    size_t startLine = line_;
    size_t startCol = column_;
    std::string content;
    
    while (pos_ < input_.size()) {
        // 检查是否遇到CHTL特征
        if (peek() == '@') {
            size_t checkPos = pos_ + 1;
            if (checkPos < input_.size() && 
                (matchAt(checkPos, "Var") || 
                 matchAt(checkPos, "Element") || 
                 matchAt(checkPos, "Style"))) {
                
                // 保存当前JS片段
                if (!content.empty()) {
                    addFragment(fragments, FragmentType::JS, content,
                               startLine, startCol, line_, column_);
                }
                
                // 扫描CHTL特征
                scanAtDirective(fragments);
                
                // 重新开始JS片段
                start = pos_;
                startLine = line_;
                startCol = column_;
                content.clear();
                continue;
            }
        }
        
        // 检查是否遇到CHTL JS特征
        if (scanCHTLJSFeature(fragments)) {
            // 保存当前JS片段
            if (!content.empty()) {
                addFragment(fragments, FragmentType::JS, content,
                           startLine, startCol, line_, column_);
            }
            
            // 重新开始JS片段
            start = pos_;
            startLine = line_;
            startCol = column_;
            content.clear();
            continue;
        }
        
        // 检查是否到达块结束
        if (peek() == '}' && contextStack_.size() > 1) {
            // 块结束
            break;
        }
        
        content += advance();
    }
    
    // 保存最后的JS片段
    if (!content.empty()) {
        addFragment(fragments, FragmentType::JS, content,
                   startLine, startCol, line_, column_);
    }
}

// 辅助方法实现
bool PrecisionScanner::matchExact(const std::string& pattern) {
    if (pos_ + pattern.size() > input_.size()) return false;
    
    for (size_t i = 0; i < pattern.size(); i++) {
        if (input_[pos_ + i] != pattern[i]) return false;
    }
    
    // 检查是否在单词边界
    if (pos_ + pattern.size() < input_.size()) {
        char nextChar = input_[pos_ + pattern.size()];
        if (std::isalnum(nextChar) || nextChar == '_') {
            return false;  // 不在单词边界
        }
    }
    
    return true;
}

void PrecisionScanner::skipWhitespace() {
    while (pos_ < input_.size() && std::isspace(peek())) {
        advance();
    }
}

std::string PrecisionScanner::scanIdentifier() {
    std::string result;
    
    while (pos_ < input_.size() && 
           (std::isalnum(peek()) || peek() == '_')) {
        result += advance();
    }
    
    return result;
}

std::string PrecisionScanner::scanBalancedBraces() {
    std::string result;
    int braceCount = 0;
    
    while (pos_ < input_.size()) {
        char c = peek();
        
        if (c == '{') {
            braceCount++;
        } else if (c == '}') {
            braceCount--;
            if (braceCount < 0) break;
        }
        
        result += advance();
    }
    
    return result;
}

void PrecisionScanner::pushContext(ScanMode mode) {
    ScanContext ctx;
    ctx.mode = mode;
    ctx.braceLevel = 0;
    ctx.isGlobalStyle = false;
    ctx.inElement = (mode != ScanMode::CHTL);  // 非顶层即在元素内
    contextStack_.push(ctx);
}

void PrecisionScanner::popContext() {
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
}

PrecisionScanner::ScanMode PrecisionScanner::currentMode() const {
    return contextStack_.empty() ? ScanMode::CHTL : contextStack_.top().mode;
}

bool PrecisionScanner::scanTextBlock(std::vector<CodeFragment>& fragments) {
    if (!matchExact("text")) return false;
    
    size_t start = pos_;
    size_t startLine = line_;
    size_t startCol = column_;
    
    pos_ += 4;  // "text"
    column_ += 4;
    skipWhitespace();
    
    if (peek() == '{') {
        // 收集整个text块作为CHTL片段
        std::string content = "text";
        content += scanBalancedBraces();
        
        addFragment(fragments, FragmentType::CHTL, content,
                   startLine, startCol, line_, column_);
        return true;
    }
    
    // 回退
    pos_ = start;
    line_ = startLine;
    column_ = startCol;
    return false;
}

bool PrecisionScanner::scanTemplateDirective(std::vector<CodeFragment>& fragments) {
    if (peek() != '[') return false;
    
    size_t savePos = pos_;
    size_t saveLine = line_;
    size_t saveCol = column_;
    
    advance(); // '['
    
    if (matchExact("Template]")) {
        // 收集整个[Template]指令
        size_t start = savePos;
        pos_ -= 1;  // 回到']'之前
        
        std::string content = "[Template]";
        pos_ += 9;  // "[Template]"
        column_ += 9;
        
        // 继续收集后续内容
        skipWhitespace();
        if (peek() == '@') {
            content += " ";
            content += advance(); // '@'
            content += scanIdentifier();
        }
        
        addFragment(fragments, FragmentType::CHTL, content,
                   saveLine, saveCol, line_, column_);
        return true;
    }
    
    // 回退
    pos_ = savePos;
    line_ = saveLine;
    column_ = saveCol;
    return false;
}

bool PrecisionScanner::scanCustomDirective(std::vector<CodeFragment>& fragments) {
    if (peek() != '[') return false;
    
    size_t savePos = pos_;
    size_t saveLine = line_;
    size_t saveCol = column_;
    
    advance(); // '['
    
    if (matchExact("Custom]")) {
        // 收集整个[Custom]指令
        size_t start = savePos;
        pos_ -= 1;  // 回到']'之前
        
        std::string content = "[Custom]";
        pos_ += 8;  // "[Custom]"
        column_ += 8;
        
        // 继续收集后续内容
        skipWhitespace();
        if (peek() == '@') {
            content += " ";
            content += advance(); // '@'
            content += scanIdentifier();
        }
        
        addFragment(fragments, FragmentType::CHTL, content,
                   saveLine, saveCol, line_, column_);
        return true;
    }
    
    // 回退
    pos_ = savePos;
    line_ = saveLine;
    column_ = saveCol;
    return false;
}

} // namespace scanner
} // namespace chtl