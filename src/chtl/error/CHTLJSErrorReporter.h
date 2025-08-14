#ifndef CHTL_JS_ERROR_REPORTER_H
#define CHTL_JS_ERROR_REPORTER_H

#include <string>
#include <vector>
#include <memory>
#include "../CHTLContext.h"

namespace chtl {

// CHTL JS错误类型
enum class CHTLJSErrorType {
    // 语法错误
    SyntaxError,
    
    // 引用错误
    ReferenceError,
    UndefinedVariable,
    UndefinedFunction,
    
    // 类型错误
    TypeError,
    InvalidSelector,
    InvalidMethodCall,
    
    // 运行时错误
    RuntimeError,
    DivisionByZero,
    NullPointerAccess,
    
    // 语义错误
    SemanticError,
    DuplicateDeclaration,
    ConstReassignment,
    TemporalDeadZone,
    
    // CHTL特定错误
    InvalidCHTLSyntax,
    InvalidEnhancedSelector,
    InvalidArrowSyntax,
    InvalidListenConfig,
    InvalidDelegateConfig,
    InvalidAnimateConfig,
    
    // 警告
    Warning,
    UnusedVariable,
    UnreachableCode,
    DeprecatedSyntax
};

// 错误位置信息
struct ErrorLocation {
    std::string file;
    int line;
    int column;
    std::string contextLine;  // 错误所在行的内容
    
    ErrorLocation() : line(0), column(0) {}
    ErrorLocation(const std::string& f, int l, int c) 
        : file(f), line(l), column(c) {}
};

// CHTL JS错误信息
struct CHTLJSError {
    CHTLJSErrorType type;
    std::string message;
    ErrorLocation location;
    std::string suggestion;  // 修复建议
    
    CHTLJSError(CHTLJSErrorType t, const std::string& msg)
        : type(t), message(msg) {}
        
    CHTLJSError(CHTLJSErrorType t, const std::string& msg, const ErrorLocation& loc)
        : type(t), message(msg), location(loc) {}
};

// CHTL JS错误报告器
class CHTLJSErrorReporter {
private:
    std::shared_ptr<CHTLContext> context;
    std::vector<CHTLJSError> errors;
    std::vector<CHTLJSError> warnings;
    bool treatWarningsAsErrors;
    int maxErrorCount;
    
public:
    CHTLJSErrorReporter(std::shared_ptr<CHTLContext> ctx)
        : context(ctx), treatWarningsAsErrors(false), maxErrorCount(100) {}
    
    // 报告错误
    void reportError(CHTLJSErrorType type, const std::string& message);
    void reportError(CHTLJSErrorType type, const std::string& message, 
                    const ErrorLocation& location);
    void reportError(const CHTLJSError& error);
    
    // 报告警告
    void reportWarning(const std::string& message);
    void reportWarning(const std::string& message, const ErrorLocation& location);
    
    // 添加修复建议
    void addSuggestion(const std::string& suggestion);
    
    // 格式化错误信息
    std::string formatError(const CHTLJSError& error) const;
    std::string formatLocation(const ErrorLocation& location) const;
    
    // 输出所有错误
    void printAllErrors() const;
    void printSummary() const;
    
    // 获取错误信息
    const std::vector<CHTLJSError>& getErrors() const { return errors; }
    const std::vector<CHTLJSError>& getWarnings() const { return warnings; }
    
    // 错误统计
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    int getErrorCount() const { return errors.size(); }
    int getWarningCount() const { return warnings.size(); }
    
    // 配置
    void setTreatWarningsAsErrors(bool treat) { treatWarningsAsErrors = treat; }
    void setMaxErrorCount(int count) { maxErrorCount = count; }
    
    // 清空错误
    void clear() {
        errors.clear();
        warnings.clear();
    }
    
    // 获取错误类型字符串
    static std::string getErrorTypeString(CHTLJSErrorType type);
    
    // 创建常见错误
    static CHTLJSError undefinedVariable(const std::string& varName, const ErrorLocation& loc);
    static CHTLJSError invalidSelector(const std::string& selector, const ErrorLocation& loc);
    static CHTLJSError duplicateDeclaration(const std::string& name, const ErrorLocation& loc);
    static CHTLJSError constReassignment(const std::string& varName, const ErrorLocation& loc);
};

} // namespace chtl

#endif // CHTL_JS_ERROR_REPORTER_H