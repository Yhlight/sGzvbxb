#ifndef CHTL_STATE_MACHINE_SCANNER_H
#define CHTL_STATE_MACHINE_SCANNER_H

#include "CHTLUnifiedScanner.h"
#include <stack>
#include <functional>

namespace chtl {
namespace scanner {

// 基于状态机的精确扫描器
// 通过状态转换来精确切割代码片段
class StateMachineScanner : public CHTLUnifiedScanner {
public:
    StateMachineScanner();
    
    std::vector<CodeFragment> scan(const std::string& input) override;
    
private:
    // 扫描器状态
    enum class ScanState {
        // 基础状态
        CHTL_TOP,           // CHTL顶层
        CHTL_ELEMENT,       // CHTL元素内部
        
        // Style相关状态
        STYLE_KEYWORD,      // style关键字
        STYLE_BLOCK_GLOBAL, // 全局style块（CSS处理）
        STYLE_BLOCK_LOCAL,  // 局部style块（CHTL处理）
        CSS_CONTENT,        // CSS内容
        
        // Script相关状态
        SCRIPT_KEYWORD,     // script关键字
        SCRIPT_BLOCK,       // script块
        JS_CONTENT,         // 普通JS内容
        CHTLJS_FEATURE,     // CHTL JS特征
        
        // CHTL特征状态
        CHTL_AT_DIRECTIVE,  // @Element, @Style, @Var
        CHTL_TEMPLATE,      // [Template]
        CHTL_CUSTOM,        // [Custom]
        CHTL_TEXT,          // text块
        CHTL_VAR_REF,       // 变量引用 如 ThemeColor(name)
        
        // CHTL JS特征状态
        CHTLJS_INTERPOLATION, // {{}}
        CHTLJS_CHAIN,         // ->
        CHTLJS_LISTEN,        // listen方法
        CHTLJS_ANIMATE,       // animate函数
    };
    
    // 状态上下文
    struct StateContext {
        ScanState state;
        size_t startPos;
        size_t startLine;
        size_t startColumn;
        std::string buffer;
        FragmentType fragmentType;
        int braceDepth;
        bool isGlobalScope;
    };
    
    // 状态栈
    std::stack<StateContext> stateStack_;
    
    // 当前收集的内容
    std::string currentBuffer_;
    FragmentType currentFragmentType_;
    size_t fragmentStartPos_;
    size_t fragmentStartLine_;
    size_t fragmentStartColumn_;
    
    // 状态转换表
    using StateHandler = std::function<void(std::vector<CodeFragment>&)>;
    std::unordered_map<ScanState, StateHandler> stateHandlers_;
    
    // 初始化状态机
    void initializeStateMachine();
    
    // 状态处理函数
    void handleCHTLTop(std::vector<CodeFragment>& fragments);
    void handleCHTLElement(std::vector<CodeFragment>& fragments);
    void handleStyleKeyword(std::vector<CodeFragment>& fragments);
    void handleStyleBlock(std::vector<CodeFragment>& fragments, bool isGlobal);
    void handleScriptKeyword(std::vector<CodeFragment>& fragments);
    void handleScriptBlock(std::vector<CodeFragment>& fragments);
    void handleJSContent(std::vector<CodeFragment>& fragments);
    void handleCHTLFeature(std::vector<CodeFragment>& fragments);
    void handleCHTLJSFeature(std::vector<CodeFragment>& fragments);
    
    // 状态转换
    void transitionTo(ScanState newState);
    void pushState(ScanState newState);
    void popState();
    ScanState currentState() const;
    
    // 片段管理
    void startFragment(FragmentType type);
    void appendToFragment(char c);
    void appendToFragment(const std::string& str);
    void finishFragment(std::vector<CodeFragment>& fragments);
    void flushCurrentFragment(std::vector<CodeFragment>& fragments);
    
    // 模式匹配
    bool matchKeyword(const std::string& keyword);
    bool matchSequence(const std::string& sequence);
    bool isAtWordBoundary();
    
    // 特征检测
    bool detectCHTLFeature();
    bool detectCHTLJSFeature();
    bool detectVariableReference();  // 检测 Name(param) 形式
    
    // 辅助方法
    char peekAhead(size_t offset = 0) const;
    void skipToNextFeature();
    std::string scanUntilChar(char delimiter);
    std::string scanBalanced(char open, char close);
};

} // namespace scanner
} // namespace chtl

#endif // CHTL_STATE_MACHINE_SCANNER_H