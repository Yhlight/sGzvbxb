#ifndef CHTL_ENHANCED_ERROR_H
#define CHTL_ENHANCED_ERROR_H

#include "ErrorInterface.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <regex>
#include <functional>

namespace chtl {
namespace error {

// 错误上下文信息
struct ErrorContext {
    std::string filename;
    std::string functionName;
    std::string phase;  // scanning, parsing, compiling, etc.
    std::unordered_map<std::string, std::string> additionalInfo;
};

// 代码片段展示
struct CodeSnippet {
    std::vector<std::string> lines;
    size_t errorLine;
    size_t errorColumn;
    size_t startLine;
    
    std::string format(bool useColor = true) const;
};

// 错误恢复建议
struct ErrorSuggestion {
    std::string description;
    std::string example;
    double confidence;  // 0.0 - 1.0
};

// 增强的错误信息
class EnhancedErrorInfo : public ErrorInfo {
public:
    EnhancedErrorInfo(
        ErrorLevel level,
        ErrorCategory category,
        const std::string& message,
        const SourceLocation& location
    );
    
    // 添加上下文
    void setContext(const ErrorContext& context) { context_ = context; }
    const ErrorContext& getContext() const { return context_; }
    
    // 添加代码片段
    void setCodeSnippet(const CodeSnippet& snippet) { snippet_ = snippet; }
    const CodeSnippet& getCodeSnippet() const { return snippet_; }
    
    // 添加建议
    void addSuggestion(const ErrorSuggestion& suggestion);
    const std::vector<ErrorSuggestion>& getSuggestions() const { return suggestions_; }
    
    // 格式化输出
    std::string format(bool useColor = true, bool verbose = false) const;
    
private:
    ErrorContext context_;
    CodeSnippet snippet_;
    std::vector<ErrorSuggestion> suggestions_;
};

// 错误恢复建议生成器
class ErrorSuggestionGenerator {
public:
    ErrorSuggestionGenerator();
    
    // 根据错误类型生成建议
    std::vector<ErrorSuggestion> generateSuggestions(
        const EnhancedErrorInfo& error
    );
    
private:
    // 语法错误建议
    std::vector<ErrorSuggestion> syntaxSuggestions(const std::string& message);
    
    // 语义错误建议
    std::vector<ErrorSuggestion> semanticSuggestions(const std::string& message);
    
    // 模块错误建议
    std::vector<ErrorSuggestion> moduleSuggestions(const std::string& message);
    
    // 常见错误模式
    struct ErrorPattern {
        std::regex pattern;
        std::string suggestion;
        std::string example;
    };
    
    std::vector<ErrorPattern> syntaxPatterns_;
    std::vector<ErrorPattern> semanticPatterns_;
    std::vector<ErrorPattern> modulePatterns_;
    
    void initializePatterns();
};

// 错误可视化器
class ErrorVisualizer {
public:
    ErrorVisualizer();
    
    // 设置颜色输出
    void setColorEnabled(bool enabled) { useColor_ = enabled; }
    
    // 可视化单个错误
    std::string visualize(const EnhancedErrorInfo& error);
    
    // 可视化错误列表
    std::string visualizeList(const std::vector<EnhancedErrorInfo>& errors);
    
    // 生成HTML报告
    std::string generateHTMLReport(const std::vector<EnhancedErrorInfo>& errors);
    
private:
    bool useColor_;
    
public:
    // ANSI颜色代码
    struct Colors {
        static const char* RED;
        static const char* YELLOW;
        static const char* GREEN;
        static const char* BLUE;
        static const char* MAGENTA;
        static const char* CYAN;
        static const char* RESET;
        static const char* BOLD;
        static const char* UNDERLINE;
    };
    
    // 获取错误级别颜色
    const char* getLevelColor(ErrorLevel level) const;
    
    // 格式化行号
    std::string formatLineNumber(size_t line, size_t width) const;
    
    // 生成错误标记行
    std::string generateMarkerLine(size_t column, size_t length = 1) const;
};

// 多语言支持
class ErrorLocalizer {
public:
    ErrorLocalizer();
    
    // 设置当前语言
    void setLanguage(const std::string& langCode);
    
    // 获取本地化消息
    std::string localize(const std::string& key, 
                        const std::unordered_map<std::string, std::string>& params = {});
    
    // 加载语言包
    void loadLanguagePack(const std::string& langCode, const std::string& packPath);
    
    // 支持的语言
    std::vector<std::string> getSupportedLanguages() const;
    
private:
    std::string currentLanguage_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> translations_;
    
    // 格式化消息（替换参数）
    std::string formatMessage(const std::string& template_, 
                            const std::unordered_map<std::string, std::string>& params);
};

// 增强的错误报告器
class EnhancedErrorReporter : public IErrorReporter {
public:
    EnhancedErrorReporter();
    
    void report(const ErrorInfo& error) override;
    
    // 增强报告
    void reportEnhanced(const EnhancedErrorInfo& error);
    
    // 设置选项
    void setUseColor(bool useColor) { visualizer_.setColorEnabled(useColor); }
    void setLanguage(const std::string& langCode) { localizer_.setLanguage(langCode); }
    void setVerbose(bool verbose) { verbose_ = verbose; }
    void setShowSuggestions(bool show) { showSuggestions_ = show; }
    
    // 批量报告
    void reportBatch(const std::vector<EnhancedErrorInfo>& errors);
    
    // 生成报告
    std::string generateTextReport();
    std::string generateHTMLReport();
    std::string generateJSONReport();
    
    // 实现IErrorReporter的便捷方法
    void debug(const std::string& message, const SourceLocation& loc) override;
    void info(const std::string& message, const SourceLocation& loc) override;
    void warning(const std::string& message, const SourceLocation& loc) override;
    void error(const std::string& message, const SourceLocation& loc) override;
    void fatal(const std::string& message, const SourceLocation& loc) override;
    
private:
    ErrorVisualizer visualizer_;
    ErrorLocalizer localizer_;
    ErrorSuggestionGenerator suggestionGen_;
    std::vector<EnhancedErrorInfo> errors_;
    bool verbose_;
    bool showSuggestions_;
    
    // 从文件读取代码片段
    CodeSnippet extractCodeSnippet(const std::string& filename, 
                                  size_t line, size_t column);
};

// 错误恢复机制
class ErrorRecovery {
public:
    ErrorRecovery();
    
    // 尝试从错误中恢复
    bool tryRecover(const EnhancedErrorInfo& error, std::string& recoveredCode);
    
    // 注册恢复策略
    using RecoveryStrategy = std::function<bool(const EnhancedErrorInfo&, std::string&)>;
    void registerStrategy(ErrorCategory category, RecoveryStrategy strategy);
    
private:
    std::unordered_map<ErrorCategory, std::vector<RecoveryStrategy>> strategies_;
    
    // 内置恢复策略
    bool recoverFromSyntaxError(const EnhancedErrorInfo& error, std::string& code);
    bool recoverFromMissingBrace(const EnhancedErrorInfo& error, std::string& code);
    bool recoverFromMissingSemicolon(const EnhancedErrorInfo& error, std::string& code);
};

// 错误统计分析
class ErrorAnalyzer {
public:
    ErrorAnalyzer();
    
    // 添加错误进行分析
    void addError(const EnhancedErrorInfo& error);
    
    // 统计信息
    struct Statistics {
        size_t totalErrors;
        std::unordered_map<ErrorLevel, size_t> byLevel;
        std::unordered_map<ErrorCategory, size_t> byCategory;
        std::unordered_map<std::string, size_t> byFile;
        std::vector<std::string> mostCommonErrors;
    };
    
    Statistics getStatistics() const;
    
    // 生成分析报告
    std::string generateAnalysisReport();
    
    // 获取热点文件（错误最多的文件）
    std::vector<std::pair<std::string, size_t>> getHotspotFiles(size_t limit = 10);
    
private:
    std::vector<EnhancedErrorInfo> errors_;
    
    // 分析常见错误模式
    void analyzePatterns();
};

// 全局增强错误管理器
class EnhancedErrorManager {
public:
    static EnhancedErrorManager& getInstance() {
        static EnhancedErrorManager instance;
        return instance;
    }
    
    // 获取增强报告器
    std::shared_ptr<EnhancedErrorReporter> getReporter() { return reporter_; }
    
    // 获取错误恢复器
    std::shared_ptr<ErrorRecovery> getRecovery() { return recovery_; }
    
    // 获取分析器
    std::shared_ptr<ErrorAnalyzer> getAnalyzer() { return analyzer_; }
    
    // 配置
    void configure(const std::string& configPath);
    
private:
    EnhancedErrorManager();
    
    std::shared_ptr<EnhancedErrorReporter> reporter_;
    std::shared_ptr<ErrorRecovery> recovery_;
    std::shared_ptr<ErrorAnalyzer> analyzer_;
};

} // namespace error
} // namespace chtl

#endif // CHTL_ENHANCED_ERROR_H