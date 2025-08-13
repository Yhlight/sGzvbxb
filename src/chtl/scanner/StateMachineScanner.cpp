#include "StateMachineScanner.h"
#include <cctype>
#include <algorithm>
#include <set>

namespace chtl {
namespace scanner {

StateMachineScanner::StateMachineScanner() : CHTLUnifiedScanner() {
    initializeStateMachine();
}

void StateMachineScanner::initializeStateMachine() {
    // 注册状态处理函数
    stateHandlers_[ScanState::CHTL_TOP] = [this](auto& f) { handleCHTLTop(f); };
    stateHandlers_[ScanState::CHTL_ELEMENT] = [this](auto& f) { handleCHTLElement(f); };
    stateHandlers_[ScanState::STYLE_KEYWORD] = [this](auto& f) { handleStyleKeyword(f); };
    stateHandlers_[ScanState::SCRIPT_KEYWORD] = [this](auto& f) { handleScriptKeyword(f); };
    stateHandlers_[ScanState::SCRIPT_BLOCK] = [this](auto& f) { handleScriptBlock(f); };
    stateHandlers_[ScanState::JS_CONTENT] = [this](auto& f) { handleJSContent(f); };
    stateHandlers_[ScanState::TEMPLATE_BLOCK] = [this](auto& f) { handleTemplateBlock(f); };
    stateHandlers_[ScanState::CUSTOM_BLOCK] = [this](auto& f) { handleCustomBlock(f); };
    stateHandlers_[ScanState::IMPORT_STATEMENT] = [this](auto& f) { handleImportStatement(f); };
    stateHandlers_[ScanState::NAMESPACE_BLOCK] = [this](auto& f) { handleNamespaceBlock(f); };
    stateHandlers_[ScanState::EXCEPT_CONSTRAINT] = [this](auto& f) { handleExceptConstraint(f); };
    stateHandlers_[ScanState::EXPORT_BLOCK] = [this](auto& f) { handleExportBlock(f); };
}

std::vector<CodeFragment> StateMachineScanner::scan(const std::string& input) {
    input_ = input;
    pos_ = 0;
    line_ = 1;
    column_ = 1;
    errors_.clear();
    
    std::vector<CodeFragment> fragments;
    
    // 初始状态
    StateContext initial;
    initial.state = ScanState::CHTL_TOP;
    initial.startPos = 0;
    initial.startLine = 1;
    initial.startColumn = 1;
    initial.braceDepth = 0;
    initial.isGlobalScope = true;
    stateStack_.push(initial);
    
    // 开始扫描
    while (pos_ < input_.size()) {
        auto handler = stateHandlers_.find(currentState());
        if (handler != stateHandlers_.end()) {
            handler->second(fragments);
        } else {
            // 默认处理：前进一个字符
            appendToFragment(advance());
        }
    }
    
    // 刷新最后的片段
    flushCurrentFragment(fragments);
    
    return fragments;
}

void StateMachineScanner::handleCHTLTop(std::vector<CodeFragment>& fragments) {
    // 跳过空白
    while (pos_ < input_.size() && std::isspace(peek())) {
        advance();
    }
    
    if (pos_ >= input_.size()) return;
    
    // 检测特征
    char c = peek();
    
    // 检测 style 关键字
    if (c == 's' && matchKeyword("style")) {
        flushCurrentFragment(fragments);
        transitionTo(ScanState::STYLE_KEYWORD);
        return;
    }
    
    // 检测 script 关键字
    if (c == 's' && matchKeyword("script")) {
        flushCurrentFragment(fragments);
        transitionTo(ScanState::SCRIPT_KEYWORD);
        return;
    }
    
    // 检测 text 块
    if (c == 't' && matchKeyword("text")) {
        flushCurrentFragment(fragments);
        startFragment(FragmentType::CHTL);
        appendToFragment("text");
        pos_ += 4;
        column_ += 4;
        
        // 扫描text块的内容
        skipWhitespace();
        if (peek() == '{') {
            appendToFragment(scanBalanced('{', '}'));
        }
        finishFragment(fragments);
        return;
    }
    
    // 检测 @ 指令
    if (c == '@') {
        if (detectCHTLFeature()) {
            flushCurrentFragment(fragments);
            handleCHTLFeature(fragments);
            return;
        }
    }
    
    // 检测 [Template], [Custom] 等
    if (c == '[') {
        size_t savePos = pos_;
        advance(); // '['
        
        if (matchKeyword("Template]")) {
            pos_ = savePos;  // 回退
            flushCurrentFragment(fragments);
            transitionTo(ScanState::TEMPLATE_BLOCK);
            return;
        } else if (matchKeyword("Custom]")) {
            pos_ = savePos;  // 回退
            flushCurrentFragment(fragments);
            transitionTo(ScanState::CUSTOM_BLOCK);
            return;
        } else if (matchKeyword("Import]")) {
            pos_ = savePos;  // 回退
            flushCurrentFragment(fragments);
            transitionTo(ScanState::IMPORT_STATEMENT);
            return;
        } else if (matchKeyword("Namespace]")) {
            pos_ = savePos;  // 回退
            flushCurrentFragment(fragments);
            transitionTo(ScanState::NAMESPACE_BLOCK);
            return;
        } else if (matchKeyword("Export]")) {
            pos_ = savePos;  // 回退
            flushCurrentFragment(fragments);
            transitionTo(ScanState::EXPORT_BLOCK);
            return;
        } else if (matchKeyword("Configuration]")) {
            pos_ = savePos;  // 回退
            flushCurrentFragment(fragments);
            handleCHTLFeature(fragments);
            return;
        }
        
        pos_ = savePos;  // 回退
    }
    
    // 检测 except 约束
    if (c == 'e' && matchKeyword("except")) {
        flushCurrentFragment(fragments);
        transitionTo(ScanState::EXCEPT_CONSTRAINT);
        return;
    }
    
    // 检测变量引用 Name(param)
    if (std::isalpha(c) && detectVariableReference()) {
        flushCurrentFragment(fragments);
        handleCHTLFeature(fragments);
        return;
    }
    
    // 检测HTML元素
    if (std::isalpha(c)) {
        std::string word = peekWord();
        if (isHTMLElement(word)) {
            flushCurrentFragment(fragments);
            pushState(ScanState::CHTL_ELEMENT);
        }
    }
    
    // 默认：作为CHTL内容收集
    if (currentFragmentType_ != FragmentType::CHTL) {
        startFragment(FragmentType::CHTL);
    }
    appendToFragment(advance());
}

void StateMachineScanner::handleStyleKeyword(std::vector<CodeFragment>& fragments) {
    // style 关键字已经被检测到
    startFragment(FragmentType::CHTL);
    appendToFragment("style");
    pos_ += 5;
    column_ += 5;
    finishFragment(fragments);
    
    skipWhitespace();
    
    if (peek() == '{') {
        advance(); // '{'
        
        // 判断是全局还是局部style
        bool isGlobal = stateStack_.size() == 1;  // 只有顶层状态
        
        if (isGlobal) {
            transitionTo(ScanState::STYLE_BLOCK_GLOBAL);
            startFragment(FragmentType::CSS);
        } else {
            transitionTo(ScanState::STYLE_BLOCK_LOCAL);
            startFragment(FragmentType::CHTL);
        }
    } else {
        // 没有跟随 {，回到之前的状态
        popState();
    }
}

void StateMachineScanner::handleScriptKeyword(std::vector<CodeFragment>& fragments) {
    // script 关键字
    startFragment(FragmentType::CHTL);
    appendToFragment("script");
    pos_ += 6;
    column_ += 6;
    finishFragment(fragments);
    
    skipWhitespace();
    
    if (peek() == '{') {
        advance(); // '{'
        transitionTo(ScanState::SCRIPT_BLOCK);
    } else {
        popState();
    }
}

void StateMachineScanner::handleScriptBlock(std::vector<CodeFragment>& fragments) {
    // 在script块内，需要检测各种特征
    
    skipWhitespace();
    if (pos_ >= input_.size()) return;
    
    // 检查块结束
    if (peek() == '}') {
        flushCurrentFragment(fragments);
        advance(); // '}'
        popState();
        return;
    }
    
    // 检测CHTL JS特征
    if (detectCHTLJSFeature()) {
        flushCurrentFragment(fragments);
        handleCHTLJSFeature(fragments);
        return;
    }
    
    // 检测CHTL特征（变量引用等）
    if (detectCHTLFeature()) {
        flushCurrentFragment(fragments);
        handleCHTLFeature(fragments);
        return;
    }
    
    // 默认作为JS内容
    if (currentFragmentType_ != FragmentType::JS) {
        startFragment(FragmentType::JS);
    }
    appendToFragment(advance());
}

void StateMachineScanner::handleCHTLFeature(std::vector<CodeFragment>& fragments) {
    startFragment(FragmentType::CHTL);
    
    // @ 指令
    if (peek() == '@') {
        appendToFragment(advance()); // '@'
        std::string directive = scanIdentifier();
        appendToFragment(directive);
        
        // 如果有参数
        skipWhitespace();
        if (peek() == '(') {
            appendToFragment(scanBalanced('(', ')'));
        }
    }
    // [Template] 等
    else if (peek() == '[') {
        appendToFragment(advance()); // '['
        while (pos_ < input_.size() && peek() != ']') {
            appendToFragment(advance());
        }
        if (peek() == ']') {
            appendToFragment(advance()); // ']'
        }
        
        // 后续的 @Element 等
        skipWhitespace();
        if (peek() == '@') {
            appendToFragment(advance());
            appendToFragment(scanIdentifier());
        }
    }
    // 变量引用 ThemeColor(param)
    else if (std::isalpha(peek())) {
        std::string name = scanIdentifier();
        appendToFragment(name);
        
        if (peek() == '(') {
            appendToFragment(scanBalanced('(', ')'));
        }
    }
    
    finishFragment(fragments);
}

void StateMachineScanner::handleCHTLJSFeature(std::vector<CodeFragment>& fragments) {
    startFragment(FragmentType::CHTL_JS);
    
    // {{ }}
    if (peek() == '{' && peekNext() == '{') {
        appendToFragment("{{");
        advance(); advance();
        
        while (pos_ < input_.size()) {
            if (peek() == '}' && peekNext() == '}') {
                appendToFragment("}}");
                advance(); advance();
                break;
            }
            appendToFragment(advance());
        }
    }
    // ->
    else if (peek() == '-' && peekNext() == '>') {
        appendToFragment("->");
        advance(); advance();
    }
    // .listen( 或 listen(
    else if (matchSequence("listen(") || matchSequence(".listen(")) {
        if (peek() == '.') {
            appendToFragment(advance());
        }
        appendToFragment("listen");
        pos_ += 6;
        column_ += 6;
        appendToFragment(scanBalanced('(', ')'));
    }
    // animate(
    else if (matchSequence("animate(")) {
        appendToFragment("animate");
        pos_ += 7;
        column_ += 7;
        appendToFragment(scanBalanced('(', ')'));
    }
    
    finishFragment(fragments);
}

// 状态管理
void StateMachineScanner::transitionTo(ScanState newState) {
    stateStack_.top().state = newState;
}

void StateMachineScanner::pushState(ScanState newState) {
    StateContext ctx;
    ctx.state = newState;
    ctx.startPos = pos_;
    ctx.startLine = line_;
    ctx.startColumn = column_;
    ctx.braceDepth = 0;
    ctx.isGlobalScope = false;
    stateStack_.push(ctx);
}

void StateMachineScanner::popState() {
    if (stateStack_.size() > 1) {
        stateStack_.pop();
    }
}

ScanState StateMachineScanner::currentState() const {
    return stateStack_.empty() ? ScanState::CHTL_TOP : stateStack_.top().state;
}

// 片段管理
void StateMachineScanner::startFragment(FragmentType type) {
    currentBuffer_.clear();
    currentFragmentType_ = type;
    fragmentStartPos_ = pos_;
    fragmentStartLine_ = line_;
    fragmentStartColumn_ = column_;
}

void StateMachineScanner::appendToFragment(char c) {
    currentBuffer_ += c;
}

void StateMachineScanner::appendToFragment(const std::string& str) {
    currentBuffer_ += str;
}

void StateMachineScanner::finishFragment(std::vector<CodeFragment>& fragments) {
    if (!currentBuffer_.empty()) {
        addFragment(fragments, currentFragmentType_, currentBuffer_,
                   fragmentStartLine_, fragmentStartColumn_, line_, column_);
        currentBuffer_.clear();
    }
}

void StateMachineScanner::flushCurrentFragment(std::vector<CodeFragment>& fragments) {
    finishFragment(fragments);
}

// 检测函数
bool StateMachineScanner::detectCHTLFeature() {
    char c = peek();
    
    // @ 指令
    if (c == '@') {
        size_t next = pos_ + 1;
        if (next < input_.size()) {
            if (matchAt(next, "Element") || matchAt(next, "Style") || matchAt(next, "Var")) {
                return true;
            }
        }
    }
    
    // [Template] 等
    if (c == '[') {
        if (matchSequence("[Template]") || matchSequence("[Custom]") || 
            matchSequence("[Import]") || matchSequence("[Configuration]")) {
            return true;
        }
    }
    
    // 变量引用检测
    if (std::isalpha(c) && detectVariableReference()) {
        return true;
    }
    
    return false;
}

bool StateMachineScanner::detectCHTLJSFeature() {
    // {{
    if (peek() == '{' && peekNext() == '{') return true;
    
    // ->
    if (peek() == '-' && peekNext() == '>') return true;
    
    // listen(
    if (matchSequence("listen(") || matchSequence(".listen(")) return true;
    
    // animate(
    if (matchSequence("animate(")) return true;
    
    return false;
}

bool StateMachineScanner::detectVariableReference() {
    // 检测 Name(param) 形式的变量引用
    size_t savePos = pos_;
    size_t saveLine = line_;
    size_t saveCol = column_;
    
    // 扫描标识符
    if (!std::isalpha(peek())) return false;
    
    while (pos_ < input_.size() && (std::isalnum(peek()) || peek() == '_')) {
        advance();
    }
    
    // 检查是否跟随 (
    bool hasParenthesis = (peek() == '(');
    
    // 恢复位置
    pos_ = savePos;
    line_ = saveLine;
    column_ = saveCol;
    
    return hasParenthesis;
}

// 辅助方法
bool StateMachineScanner::matchKeyword(const std::string& keyword) {
    if (pos_ + keyword.size() > input_.size()) return false;
    
    for (size_t i = 0; i < keyword.size(); i++) {
        if (input_[pos_ + i] != keyword[i]) return false;
    }
    
    // 检查单词边界
    if (pos_ + keyword.size() < input_.size()) {
        char next = input_[pos_ + keyword.size()];
        if (std::isalnum(next) || next == '_') return false;
    }
    
    return true;
}

bool StateMachineScanner::matchSequence(const std::string& sequence) {
    if (pos_ + sequence.size() > input_.size()) return false;
    
    for (size_t i = 0; i < sequence.size(); i++) {
        if (input_[pos_ + i] != sequence[i]) return false;
    }
    
    return true;
}

std::string StateMachineScanner::scanIdentifier() {
    std::string result;
    while (pos_ < input_.size() && (std::isalnum(peek()) || peek() == '_')) {
        result += advance();
    }
    return result;
}

std::string StateMachineScanner::scanBalanced(char open, char close) {
    std::string result;
    int depth = 0;
    
    if (peek() == open) {
        result += advance();
        depth = 1;
    }
    
    while (pos_ < input_.size() && depth > 0) {
        char c = peek();
        if (c == open) depth++;
        else if (c == close) depth--;
        
        result += advance();
        
        if (depth == 0) break;
    }
    
    return result;
}

std::string StateMachineScanner::peekWord() {
    size_t savePos = pos_;
    std::string word;
    
    while (pos_ < input_.size() && std::isalpha(peek())) {
        word += advance();
    }
    
    pos_ = savePos;  // 恢复位置
    return word;
}

bool StateMachineScanner::isHTMLElement(const std::string& word) {
    // 简单的HTML元素检查
    static const std::set<std::string> htmlElements = {
        "html", "head", "body", "div", "span", "p", "a", "img",
        "h1", "h2", "h3", "h4", "h5", "h6", "ul", "ol", "li",
        "table", "tr", "td", "th", "form", "input", "button"
        // ... 更多HTML元素
    };
    
    return htmlElements.find(word) != htmlElements.end();
}

void StateMachineScanner::handleJSContent(std::vector<CodeFragment>& fragments) {
    // JS内容处理 - 作为状态之间的内容块
    if (currentFragmentType_ != FragmentType::JS) {
        startFragment(FragmentType::JS);
    }
    
    // 收集到下一个特征或状态改变
    while (pos_ < input_.size()) {
        // 检查是否需要状态转换
        if (peek() == '}' || detectCHTLFeature() || detectCHTLJSFeature()) {
            break;
        }
        
        appendToFragment(advance());
    }
    
    finishFragment(fragments);
}

void StateMachineScanner::handleCHTLElement(std::vector<CodeFragment>& fragments) {
    // 处理CHTL元素内部
    // 这里的逻辑类似handleCHTLTop，但是isGlobalScope = false
    handleCHTLTop(fragments);
}

void StateMachineScanner::handleStyleBlock(std::vector<CodeFragment>& fragments, bool isGlobal) {
    if (isGlobal) {
        // 全局style - CSS处理
        startFragment(FragmentType::CSS);
        
        // 收集整个块内容
        int braceDepth = 1;
        while (pos_ < input_.size() && braceDepth > 0) {
            char c = peek();
            if (c == '{') braceDepth++;
            else if (c == '}') {
                braceDepth--;
                if (braceDepth == 0) break;
            }
            appendToFragment(advance());
        }
        
        finishFragment(fragments);
    } else {
        // 局部style - CHTL处理，需要检测特征
        while (pos_ < input_.size()) {
            if (peek() == '}') {
                flushCurrentFragment(fragments);
                popState();
                return;
            }
            
            if (detectCHTLFeature()) {
                flushCurrentFragment(fragments);
                handleCHTLFeature(fragments);
            } else {
                if (currentFragmentType_ != FragmentType::CHTL) {
                    startFragment(FragmentType::CHTL);
                }
                appendToFragment(advance());
            }
        }
    }
}

char StateMachineScanner::peekAhead(size_t offset) const {
    size_t idx = pos_ + offset;
    return idx < input_.size() ? input_[idx] : '\0';
}

// 处理[Template]块
void StateMachineScanner::handleTemplateBlock(std::vector<CodeFragment>& fragments) {
    startFragment(FragmentType::CHTL);
    
    // [Template]
    appendToFragment(advance()); // '['
    while (pos_ < input_.size() && peek() != ']') {
        appendToFragment(advance());
    }
    if (peek() == ']') {
        appendToFragment(advance()); // ']'
    }
    
    skipWhitespace();
    
    // @Element / @Style / @Var
    if (peek() == '@') {
        appendToFragment(advance()); // '@'
        std::string directive = scanIdentifier();
        appendToFragment(directive);
    }
    
    skipWhitespace();
    
    // 模板名称
    std::string name = scanIdentifier();
    appendToFragment(name);
    
    skipWhitespace();
    
    // 扫描模板内容
    if (peek() == '{') {
        appendToFragment(scanBalanced('{', '}'));
    }
    
    finishFragment(fragments);
    popState();  // 回到上一个状态
}

// 处理[Custom]块
void StateMachineScanner::handleCustomBlock(std::vector<CodeFragment>& fragments) {
    startFragment(FragmentType::CHTL);
    
    // [Custom]
    appendToFragment(advance()); // '['
    while (pos_ < input_.size() && peek() != ']') {
        appendToFragment(advance());
    }
    if (peek() == ']') {
        appendToFragment(advance()); // ']'
    }
    
    skipWhitespace();
    
    // @Element / @Style / @Var
    if (peek() == '@') {
        appendToFragment(advance()); // '@'
        std::string directive = scanIdentifier();
        appendToFragment(directive);
    }
    
    skipWhitespace();
    
    // 自定义名称
    std::string name = scanIdentifier();
    appendToFragment(name);
    
    skipWhitespace();
    
    // 扫描自定义内容
    if (peek() == '{') {
        appendToFragment(scanBalanced('{', '}'));
    }
    
    finishFragment(fragments);
    popState();  // 回到上一个状态
}

// 处理[Import]语句
void StateMachineScanner::handleImportStatement(std::vector<CodeFragment>& fragments) {
    startFragment(FragmentType::CHTL);
    
    // 扫描整个import语句直到分号或换行
    appendToFragment('[');
    advance();
    
    while (pos_ < input_.size()) {
        char c = peek();
        if (c == ';' || c == '\n') {
            if (c == ';') {
                appendToFragment(advance()); // 包含分号
            }
            break;
        }
        appendToFragment(advance());
    }
    
    finishFragment(fragments);
    popState();
}

// 处理[Namespace]块
void StateMachineScanner::handleNamespaceBlock(std::vector<CodeFragment>& fragments) {
    startFragment(FragmentType::CHTL);
    
    // [Namespace]
    appendToFragment(advance()); // '['
    while (pos_ < input_.size() && peek() != ']') {
        appendToFragment(advance());
    }
    if (peek() == ']') {
        appendToFragment(advance()); // ']'
    }
    
    skipWhitespace();
    
    // 命名空间名称
    std::string name = scanIdentifier();
    appendToFragment(name);
    
    skipWhitespace();
    
    // 可能有嵌套命名空间或内容块
    if (peek() == '{') {
        // 有花括号，扫描整个块
        appendToFragment(scanBalanced('{', '}'));
    } else {
        // 可能是嵌套命名空间的简写形式
        // 继续扫描到下一个有意义的内容
        while (pos_ < input_.size() && std::isspace(peek())) {
            advance();
        }
    }
    
    finishFragment(fragments);
    popState();
}

// 处理except约束
void StateMachineScanner::handleExceptConstraint(std::vector<CodeFragment>& fragments) {
    startFragment(FragmentType::CHTL);
    
    // except关键字
    appendToFragment("except");
    pos_ += 6;
    column_ += 6;
    
    skipWhitespace();
    
    // 扫描约束内容直到分号或下一个语句
    while (pos_ < input_.size()) {
        char c = peek();
        if (c == ';' || c == '\n' || c == '{' || c == '}') {
            if (c == ';') {
                appendToFragment(advance()); // 包含分号
            }
            break;
        }
        appendToFragment(advance());
    }
    
    finishFragment(fragments);
    popState();
}

// 处理[Export]块
void StateMachineScanner::handleExportBlock(std::vector<CodeFragment>& fragments) {
    startFragment(FragmentType::CHTL);
    
    // [Export]
    appendToFragment(advance()); // '['
    while (pos_ < input_.size() && peek() != ']') {
        appendToFragment(advance());
    }
    if (peek() == ']') {
        appendToFragment(advance()); // ']'
    }
    
    skipWhitespace();
    
    // 扫描export内容
    if (peek() == '{') {
        appendToFragment(scanBalanced('{', '}'));
    }
    
    finishFragment(fragments);
    popState();
}

} // namespace scanner
} // namespace chtl