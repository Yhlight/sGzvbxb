#pragma once

#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace scanner {

// 代码片段类型
enum class FragmentType {
    CHTL,       // 纯 CHTL 代码
    CHTL_JS,    // CHTL JS 代码（包含 {{}} 和 -> 等特性）
    CSS,        // 纯 CSS 代码
    JAVASCRIPT  // 纯 JavaScript 代码
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    
    // 用于调试
    std::string toString() const;
};

// 扫描器状态
enum class ScannerState {
    CHTL,           // 默认状态，处理 CHTL
    IN_STYLE,       // 在 style 块中
    IN_SCRIPT,      // 在 script 块中
    IN_STRING,      // 在字符串中
    IN_COMMENT,     // 在注释中
    IN_ORIGIN,      // 在原始嵌入块中
};

// 统一扫描器 - 精准代码切割
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    
    // 扫描源代码，返回代码片段
    std::vector<CodeFragment> scan(const std::string& source);
    
    // 配置选项
    void setDebugMode(bool debug) { debugMode_ = debug; }
    
private:
    // 状态管理
    ScannerState currentState_;
    std::vector<ScannerState> stateStack_;
    
    // 位置跟踪
    size_t currentLine_;
    size_t currentColumn_;
    size_t currentPos_;
    
    // 源代码
    std::string source_;
    
    // 结果
    std::vector<CodeFragment> fragments_;
    
    // 临时缓冲区
    std::string currentBuffer_;
    size_t fragmentStartLine_;
    size_t fragmentStartColumn_;
    
    // 配置
    bool debugMode_;
    
    // 状态管理方法
    void pushState(ScannerState state);
    void popState();
    
    // 片段管理方法
    void startFragment(FragmentType type);
    void endFragment();
    void appendToFragment(char ch);
    void appendToFragment(const std::string& str);
    
    // 扫描方法
    void scanCHTL();
    void scanStyle();
    void scanScript();
    void scanString(char quote);
    void scanComment();
    void scanOrigin();
    
    // 辅助方法
    bool matchKeyword(const std::string& keyword);
    bool matchSequence(const std::string& seq);
    bool isAtEnd() const;
    char peek(int offset = 0) const;
    char advance();
    void skipWhitespace();
    
    // 特殊检测方法
    bool isCHTLJSFeature(); // 检测 {{}} 或 ->
    bool isInCHTLJSContext(); // 判断是否在 CHTL JS 上下文中
    bool isCHTLJSContinuation(); // 判断是否是 CHTL JS 的延续
    
    // CHTL JS 专有方法
    bool isCHTLJSMethod(const std::string& method);
    std::string peekIdentifier();
    std::string peekIdentifierAt(size_t pos);
    void scanIdentifier();
    void scanBalancedParentheses();
    void scanCHTLJS();
    
    // 位置更新
    void updatePosition(char ch);
};

} // namespace scanner
} // namespace chtl