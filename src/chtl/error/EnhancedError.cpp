#include "EnhancedError.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace chtl {
namespace error {

// 辅助函数
std::string toString(ErrorLevel level) {
    switch (level) {
        case ErrorLevel::WARNING: return "WARNING";
        case ErrorLevel::ERROR: return "ERROR";
        case ErrorLevel::FATAL: return "FATAL";
        case ErrorLevel::INFO: return "INFO";
        default: return "UNKNOWN";
    }
}

// ANSI颜色代码定义
const char* ErrorVisualizer::Colors::RED = "\033[31m";
const char* ErrorVisualizer::Colors::YELLOW = "\033[33m";
const char* ErrorVisualizer::Colors::GREEN = "\033[32m";
const char* ErrorVisualizer::Colors::BLUE = "\033[34m";
const char* ErrorVisualizer::Colors::MAGENTA = "\033[35m";
const char* ErrorVisualizer::Colors::CYAN = "\033[36m";
const char* ErrorVisualizer::Colors::RESET = "\033[0m";
const char* ErrorVisualizer::Colors::BOLD = "\033[1m";
const char* ErrorVisualizer::Colors::UNDERLINE = "\033[4m";

// CodeSnippet 实现
std::string CodeSnippet::format(bool useColor) const {
    std::stringstream ss;
    
    // 计算行号宽度
    size_t maxLineNum = startLine + lines.size() - 1;
    size_t lineNumWidth = std::to_string(maxLineNum).length();
    
    for (size_t i = 0; i < lines.size(); ++i) {
        size_t lineNum = startLine + i;
        bool isErrorLine = (lineNum == errorLine);
        
        // 行号
        if (useColor && isErrorLine) {
            ss << ErrorVisualizer::Colors::RED << ErrorVisualizer::Colors::BOLD;
        }
        ss << std::setw(lineNumWidth) << lineNum << " | ";
        if (useColor && isErrorLine) {
            ss << ErrorVisualizer::Colors::RESET;
        }
        
        // 代码行
        ss << lines[i] << "\n";
        
        // 错误标记
        if (isErrorLine && errorColumn > 0) {
            ss << std::string(lineNumWidth + 3, ' ');  // 对齐
            
            if (useColor) {
                ss << ErrorVisualizer::Colors::RED << ErrorVisualizer::Colors::BOLD;
            }
            
            // 生成错误指示符
            if (errorColumn <= lines[i].length()) {
                ss << std::string(errorColumn - 1, ' ') << "^";
                
                // 尝试标记整个词
                size_t pos = errorColumn - 1;
                while (pos < lines[i].length() && 
                       (std::isalnum(lines[i][pos]) || lines[i][pos] == '_')) {
                    ss << "~";
                    pos++;
                }
            }
            
            if (useColor) {
                ss << ErrorVisualizer::Colors::RESET;
            }
            ss << "\n";
        }
    }
    
    return ss.str();
}

// EnhancedErrorInfo 实现
EnhancedErrorInfo::EnhancedErrorInfo(
    ErrorLevel level,
    ErrorCategory category,
    const std::string& message,
    const SourceLocation& location)
    : ErrorInfo(level, category, message) {
    this->location = location;
}

void EnhancedErrorInfo::addSuggestion(const ErrorSuggestion& suggestion) {
    suggestions_.push_back(suggestion);
    // 按置信度排序
    std::sort(suggestions_.begin(), suggestions_.end(),
              [](const ErrorSuggestion& a, const ErrorSuggestion& b) {
                  return a.confidence > b.confidence;
              });
}

std::string EnhancedErrorInfo::format(bool useColor, bool verbose) const {
    std::stringstream ss;
    
    // 错误级别和位置
    if (useColor) {
        const char* color = "";
        switch (level) {
            case ErrorLevel::Fatal:
            case ErrorLevel::Error:
                color = ErrorVisualizer::Colors::RED;
                break;
            case ErrorLevel::Warning:
                color = ErrorVisualizer::Colors::YELLOW;
                break;
            case ErrorLevel::Info:
                color = ErrorVisualizer::Colors::CYAN;
                break;
            default:
                color = ErrorVisualizer::Colors::RESET;
        }
        ss << color << ErrorVisualizer::Colors::BOLD;
    }
    
    ss << "[" << toString(level) << "] ";
    
    if (useColor) {
        ss << ErrorVisualizer::Colors::RESET;
    }
    
    ss << location.filename << ":" << location.line << ":" << location.column << ": ";
    ss << message << "\n";
    
    // 代码片段
    if (!snippet_.lines.empty()) {
        ss << "\n" << snippet_.format(useColor);
    }
    
    // 建议
    if (!suggestions_.empty()) {
        ss << "\n";
        if (useColor) {
            ss << ErrorVisualizer::Colors::GREEN << ErrorVisualizer::Colors::BOLD;
        }
        ss << "建议:\n";
        if (useColor) {
            ss << ErrorVisualizer::Colors::RESET;
        }
        
        for (size_t i = 0; i < suggestions_.size() && i < 3; ++i) {
            ss << "  " << (i + 1) << ". " << suggestions_[i].description << "\n";
            if (!suggestions_[i].example.empty()) {
                ss << "     示例: " << suggestions_[i].example << "\n";
            }
        }
    }
    
    // 详细信息
    if (verbose && !context_.additionalInfo.empty()) {
        ss << "\n";
        if (useColor) {
            ss << ErrorVisualizer::Colors::MAGENTA;
        }
        ss << "上下文:\n";
        if (useColor) {
            ss << ErrorVisualizer::Colors::RESET;
        }
        
        for (const auto& [key, value] : context_.additionalInfo) {
            ss << "  " << key << ": " << value << "\n";
        }
    }
    
    return ss.str();
}

// ErrorSuggestionGenerator 实现
ErrorSuggestionGenerator::ErrorSuggestionGenerator() {
    initializePatterns();
}

void ErrorSuggestionGenerator::initializePatterns() {
    // CHTL语法错误模式
    syntaxPatterns_.push_back({
        std::regex("缺少.*[}\\]]"),
        "检查是否有未闭合的花括号或方括号",
        "div { style { color: red; } }"  // 注意闭合所有括号
    });
    
    syntaxPatterns_.push_back({
        std::regex("意外的.*@Var"),
        "变量引用应使用 VariableName(param) 形式，而不是 @Var()",
        "color: ThemeColor(primary);"
    });
    
    syntaxPatterns_.push_back({
        std::regex("未知的指令.*@"),
        "CHTL支持的指令有 @Element, @Style, @Var, @Html, @JavaScript等",
        "[Template] @Element Button { ... }"
    });
    
    syntaxPatterns_.push_back({
        std::regex(".*style.*不能.*全局"),
        "全局style块不支持CHTL扩展，请在元素内使用局部style块",
        "div { style { .auto-class { ... } } }"
    });
    
    // 模块错误模式
    modulePatterns_.push_back({
        std::regex("找不到模块"),
        "检查文件路径是否正确，可以使用.表示/",
        "[Import] @Element Button from ./components.Button.chtl;"
    });
    
    modulePatterns_.push_back({
        std::regex("循环依赖"),
        "模块之间存在循环引用，考虑重构代码结构",
        "将共享部分提取到独立模块中"
    });
    
    // 语义错误模式
    semanticPatterns_.push_back({
        std::regex("重复定义"),
        "同一作用域内不能有重复的名称定义",
        "使用不同的名称或命名空间"
    });
    
    semanticPatterns_.push_back({
        std::regex("未定义.*引用"),
        "使用前需要先定义或导入",
        "[Import] [Template] @Style ButtonStyle from ./styles.chtl;"
    });
}

std::vector<ErrorSuggestion> ErrorSuggestionGenerator::generateSuggestions(
    const EnhancedErrorInfo& error) {
    
    std::vector<ErrorSuggestion> suggestions;
    
    switch (error.category) {
        case ErrorCategory::Syntax:
            suggestions = syntaxSuggestions(error.message);
            break;
        case ErrorCategory::Semantic:
            suggestions = semanticSuggestions(error.message);
            break;
        case ErrorCategory::ModuleSystem:
            suggestions = moduleSuggestions(error.message);
            break;
        default:
            break;
    }
    
    return suggestions;
}

std::vector<ErrorSuggestion> ErrorSuggestionGenerator::syntaxSuggestions(
    const std::string& message) {
    
    std::vector<ErrorSuggestion> suggestions;
    
    for (const auto& pattern : syntaxPatterns_) {
        if (std::regex_search(message, pattern.pattern)) {
            suggestions.push_back({
                pattern.suggestion,
                pattern.example,
                0.8  // 高置信度
            });
        }
    }
    
    // 通用建议
    if (suggestions.empty()) {
        suggestions.push_back({
            "检查CHTL语法是否正确",
            "参考CHTL语法文档",
            0.3
        });
    }
    
    return suggestions;
}

std::vector<ErrorSuggestion> ErrorSuggestionGenerator::semanticSuggestions(
    const std::string& message) {
    
    std::vector<ErrorSuggestion> suggestions;
    
    for (const auto& pattern : semanticPatterns_) {
        if (std::regex_search(message, pattern.pattern)) {
            suggestions.push_back({
                pattern.suggestion,
                pattern.example,
                0.7
            });
        }
    }
    
    return suggestions;
}

std::vector<ErrorSuggestion> ErrorSuggestionGenerator::moduleSuggestions(
    const std::string& message) {
    
    std::vector<ErrorSuggestion> suggestions;
    
    for (const auto& pattern : modulePatterns_) {
        if (std::regex_search(message, pattern.pattern)) {
            suggestions.push_back({
                pattern.suggestion,
                pattern.example,
                0.9
            });
        }
    }
    
    return suggestions;
}

// ErrorRecovery 实现
ErrorRecovery::ErrorRecovery() {
    // 注册内置恢复策略
    registerStrategy(ErrorCategory::Syntax, 
        [this](const EnhancedErrorInfo& e, std::string& c) {
            return recoverFromSyntaxError(e, c);
        });
}

bool ErrorRecovery::tryRecover(const EnhancedErrorInfo& error, std::string& recoveredCode) {
    auto it = strategies_.find(error.category);
    if (it != strategies_.end()) {
        for (const auto& strategy : it->second) {
            if (strategy(error, recoveredCode)) {
                return true;
            }
        }
    }
    return false;
}

void ErrorRecovery::registerStrategy(ErrorCategory category, RecoveryStrategy strategy) {
    strategies_[category].push_back(strategy);
}

bool ErrorRecovery::recoverFromSyntaxError(const EnhancedErrorInfo& error, std::string& code) {
    // 尝试从特定的语法错误恢复
    if (error.message.find("缺少") != std::string::npos) {
        if (error.message.find("}") != std::string::npos) {
            return recoverFromMissingBrace(error, code);
        }
        if (error.message.find(";") != std::string::npos) {
            return recoverFromMissingSemicolon(error, code);
        }
    }
    return false;
}

bool ErrorRecovery::recoverFromMissingBrace(const EnhancedErrorInfo& error, std::string& code) {
    // 简单的策略：在错误位置插入缺失的括号
    // 实际实现应该更智能
    code += " }";
    return true;
}

bool ErrorRecovery::recoverFromMissingSemicolon(const EnhancedErrorInfo& error, std::string& code) {
    // 在行尾添加分号
    code += ";";
    return true;
}

// ErrorAnalyzer 实现
ErrorAnalyzer::ErrorAnalyzer() {}

void ErrorAnalyzer::addError(const EnhancedErrorInfo& error) {
    errors_.push_back(error);
}

ErrorAnalyzer::Statistics ErrorAnalyzer::getStatistics() const {
    Statistics stats;
    stats.totalErrors = errors_.size();
    
    for (const auto& error : errors_) {
        stats.byLevel[error.level]++;
        stats.byCategory[error.category]++;
        stats.byFile[error.location.filename]++;
    }
    
    // 分析最常见的错误
    std::unordered_map<std::string, size_t> messageCount;
    for (const auto& error : errors_) {
        messageCount[error.message]++;
    }
    
    // 排序找出最常见的
    std::vector<std::pair<std::string, size_t>> sorted(messageCount.begin(), messageCount.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < 5 && i < sorted.size(); ++i) {
        stats.mostCommonErrors.push_back(sorted[i].first);
    }
    
    return stats;
}

std::string ErrorAnalyzer::generateAnalysisReport() {
    auto stats = getStatistics();
    std::stringstream report;
    
    report << "=== CHTL错误分析报告 ===\n\n";
    report << "总错误数: " << stats.totalErrors << "\n\n";
    
    report << "按级别分类:\n";
    for (const auto& [level, count] : stats.byLevel) {
        report << "  " << toString(level) << ": " << count << "\n";
    }
    
    report << "\n按类别分类:\n";
    for (const auto& [category, count] : stats.byCategory) {
        report << "  " << toString(category) << ": " << count << "\n";
    }
    
    report << "\n最常见的错误:\n";
    for (size_t i = 0; i < stats.mostCommonErrors.size(); ++i) {
        report << "  " << (i + 1) << ". " << stats.mostCommonErrors[i] << "\n";
    }
    
    report << "\n热点文件:\n";
    auto hotspots = getHotspotFiles(5);
    for (const auto& [file, count] : hotspots) {
        report << "  " << file << ": " << count << " 个错误\n";
    }
    
    return report.str();
}

std::vector<std::pair<std::string, size_t>> ErrorAnalyzer::getHotspotFiles(size_t limit) {
    std::unordered_map<std::string, size_t> fileErrors;
    for (const auto& error : errors_) {
        fileErrors[error.location.filename]++;
    }
    
    std::vector<std::pair<std::string, size_t>> sorted(fileErrors.begin(), fileErrors.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    if (sorted.size() > limit) {
        sorted.resize(limit);
    }
    
    return sorted;
}

// ErrorVisualizer 实现
ErrorVisualizer::ErrorVisualizer() : useColor_(true) {}

const char* ErrorVisualizer::getLevelColor(ErrorLevel level) const {
    switch (level) {
        case ErrorLevel::Fatal:
        case ErrorLevel::Error:
            return Colors::RED;
        case ErrorLevel::Warning:
            return Colors::YELLOW;
        case ErrorLevel::Info:
            return Colors::CYAN;
        case ErrorLevel::Debug:
            return Colors::MAGENTA;
    }
    return Colors::RESET;
}

std::string ErrorVisualizer::visualize(const EnhancedErrorInfo& error) {
    return error.format(useColor_, true);
}

std::string ErrorVisualizer::visualizeList(const std::vector<EnhancedErrorInfo>& errors) {
    std::stringstream ss;
    
    // 按文件分组
    std::unordered_map<std::string, std::vector<const EnhancedErrorInfo*>> byFile;
    for (const auto& error : errors) {
        byFile[error.location.filename].push_back(&error);
    }
    
    for (const auto& [filename, fileErrors] : byFile) {
        if (useColor_) {
            ss << Colors::BOLD << Colors::UNDERLINE;
        }
        ss << filename << ":\n";
        if (useColor_) {
            ss << Colors::RESET;
        }
        
        for (const auto* error : fileErrors) {
            ss << visualize(*error) << "\n";
        }
    }
    
    // 统计
    size_t errorCount = 0, warningCount = 0;
    for (const auto& error : errors) {
        if (error.level == ErrorLevel::Error || error.level == ErrorLevel::Fatal) {
            errorCount++;
        } else if (error.level == ErrorLevel::Warning) {
            warningCount++;
        }
    }
    
    ss << "\n";
    if (useColor_) {
        ss << Colors::BOLD;
    }
    ss << "总计: ";
    
    if (errorCount > 0) {
        if (useColor_) ss << Colors::RED;
        ss << errorCount << " 个错误";
        if (useColor_) ss << Colors::RESET;
    }
    
    if (errorCount > 0 && warningCount > 0) {
        ss << ", ";
    }
    
    if (warningCount > 0) {
        if (useColor_) ss << Colors::YELLOW;
        ss << warningCount << " 个警告";
        if (useColor_) ss << Colors::RESET;
    }
    
    ss << "\n";
    
    return ss.str();
}

std::string ErrorVisualizer::generateHTMLReport(const std::vector<EnhancedErrorInfo>& errors) {
    std::stringstream html;
    
    html << R"(<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>CHTL编译错误报告</title>
    <style>
        body { font-family: Arial, sans-serif; background: #f5f5f5; margin: 20px; }
        .container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .error { border-left: 4px solid #f44336; padding: 10px; margin: 10px 0; background: #ffebee; }
        .warning { border-left: 4px solid #ff9800; padding: 10px; margin: 10px 0; background: #fff3e0; }
        .info { border-left: 4px solid #2196f3; padding: 10px; margin: 10px 0; background: #e3f2fd; }
        .code { font-family: 'Courier New', monospace; background: #f5f5f5; padding: 10px; border-radius: 4px; overflow-x: auto; }
        .line-error { background: #ffcdd2; }
        .suggestion { color: #4caf50; margin-top: 10px; }
        .file-group { margin: 20px 0; }
        .file-header { font-size: 18px; font-weight: bold; color: #333; margin-bottom: 10px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>CHTL编译错误报告</h1>
)";
    
    // 按文件分组
    std::unordered_map<std::string, std::vector<const EnhancedErrorInfo*>> byFile;
    for (const auto& error : errors) {
        byFile[error.location.filename].push_back(&error);
    }
    
    for (const auto& [filename, fileErrors] : byFile) {
        html << "<div class='file-group'>";
        html << "<div class='file-header'>" << filename << "</div>";
        
        for (const auto* error : fileErrors) {
            std::string className = "info";
            if (error->level == ErrorLevel::Error || error->level == ErrorLevel::Fatal) {
                className = "error";
            } else if (error->level == ErrorLevel::Warning) {
                className = "warning";
            }
            
            html << "<div class='" << className << "'>";
            html << "<strong>[" << toString(error->level) << "]</strong> ";
            html << "行 " << error->location.line << ", 列 " << error->location.column << ": ";
            html << error->message;
            
            // 代码片段
            if (!error->getCodeSnippet().lines.empty()) {
                html << "<div class='code'><pre>";
                const auto& snippet = error->getCodeSnippet();
                for (size_t i = 0; i < snippet.lines.size(); ++i) {
                    size_t lineNum = snippet.startLine + i;
                    bool isError = (lineNum == snippet.errorLine);
                    
                    if (isError) html << "<span class='line-error'>";
                    html << std::setw(4) << lineNum << " | " << snippet.lines[i];
                    if (isError) html << "</span>";
                    html << "\n";
                }
                html << "</pre></div>";
            }
            
            // 建议
            if (!error->getSuggestions().empty()) {
                html << "<div class='suggestion'><strong>建议:</strong><ul>";
                for (const auto& suggestion : error->getSuggestions()) {
                    html << "<li>" << suggestion.description;
                    if (!suggestion.example.empty()) {
                        html << "<br><code>" << suggestion.example << "</code>";
                    }
                    html << "</li>";
                }
                html << "</ul></div>";
            }
            
            html << "</div>";
        }
        
        html << "</div>";
    }
    
    html << "</div></body></html>";
    
    return html.str();
}

// ErrorLocalizer 实现
ErrorLocalizer::ErrorLocalizer() : currentLanguage_("zh-CN") {
    // 加载默认的中文语言包
    translations_["zh-CN"]["error.syntax.missing_brace"] = "缺少闭合括号 '{0}'";
    translations_["zh-CN"]["error.undefined_reference"] = "未定义的引用: {name}";
    translations_["zh-CN"]["error.module_not_found"] = "找不到模块: {path}";
    
    // 英文语言包
    translations_["en-US"]["error.syntax.missing_brace"] = "Missing closing brace '{0}'";
    translations_["en-US"]["error.undefined_reference"] = "Undefined reference: {name}";
    translations_["en-US"]["error.module_not_found"] = "Module not found: {path}";
}

void ErrorLocalizer::setLanguage(const std::string& langCode) {
    if (translations_.find(langCode) != translations_.end()) {
        currentLanguage_ = langCode;
    }
}

std::string ErrorLocalizer::localize(const std::string& key,
                                   const std::unordered_map<std::string, std::string>& params) {
    auto langIt = translations_.find(currentLanguage_);
    if (langIt == translations_.end()) {
        return key;  // 返回键作为后备
    }
    
    auto msgIt = langIt->second.find(key);
    if (msgIt == langIt->second.end()) {
        return key;  // 返回键作为后备
    }
    
    return formatMessage(msgIt->second, params);
}

std::string ErrorLocalizer::formatMessage(const std::string& template_,
                                        const std::unordered_map<std::string, std::string>& params) {
    std::string result = template_;
    
    // 替换命名参数 {name}
    for (const auto& [key, value] : params) {
        std::string placeholder = "{" + key + "}";
        size_t pos = result.find(placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos = result.find(placeholder, pos + value.length());
        }
    }
    
    return result;
}

// EnhancedErrorReporter 实现
EnhancedErrorReporter::EnhancedErrorReporter() 
    : verbose_(false), showSuggestions_(true) {
    localizer_.setLanguage("zh-CN");  // 默认中文
}

void EnhancedErrorReporter::report(const ErrorInfo& error) {
    // 转换为增强错误信息
    EnhancedErrorInfo enhanced(error.level, error.category, error.message, error.location);
    
    // 提取代码片段
    if (!error.location.filename.empty() && error.location.line > 0) {
        enhanced.setCodeSnippet(extractCodeSnippet(
            error.location.filename, 
            error.location.line, 
            error.location.column
        ));
    }
    
    // 生成建议
    if (showSuggestions_) {
        auto suggestions = suggestionGen_.generateSuggestions(enhanced);
        for (const auto& suggestion : suggestions) {
            enhanced.addSuggestion(suggestion);
        }
    }
    
    reportEnhanced(enhanced);
}

void EnhancedErrorReporter::reportEnhanced(const EnhancedErrorInfo& error) {
    errors_.push_back(error);
    
    // 立即输出到控制台
    std::cerr << visualizer_.visualize(error) << std::endl;
}

CodeSnippet EnhancedErrorReporter::extractCodeSnippet(const std::string& filename,
                                                     size_t line, size_t column) {
    CodeSnippet snippet;
    snippet.errorLine = line;
    snippet.errorColumn = column;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return snippet;
    }
    
    // 读取错误行前后的几行
    const size_t contextLines = 2;
    size_t startLine = line > contextLines ? line - contextLines : 1;
    size_t endLine = line + contextLines;
    
    snippet.startLine = startLine;
    
    std::string currentLine;
    size_t currentLineNum = 1;
    
    while (std::getline(file, currentLine)) {
        if (currentLineNum >= startLine && currentLineNum <= endLine) {
            snippet.lines.push_back(currentLine);
        }
        if (currentLineNum > endLine) {
            break;
        }
        currentLineNum++;
    }
    
    return snippet;
}

void EnhancedErrorReporter::debug(const std::string& message, const SourceLocation& loc) {
    report(ErrorInfo(ErrorLevel::Debug, ErrorCategory::Internal, message, loc));
}

void EnhancedErrorReporter::info(const std::string& message, const SourceLocation& loc) {
    report(ErrorInfo(ErrorLevel::Info, ErrorCategory::Internal, message, loc));
}

void EnhancedErrorReporter::warning(const std::string& message, const SourceLocation& loc) {
    report(ErrorInfo(ErrorLevel::Warning, ErrorCategory::Internal, message, loc));
}

void EnhancedErrorReporter::error(const std::string& message, const SourceLocation& loc) {
    report(ErrorInfo(ErrorLevel::Error, ErrorCategory::Internal, message, loc));
}

void EnhancedErrorReporter::fatal(const std::string& message, const SourceLocation& loc) {
    report(ErrorInfo(ErrorLevel::Fatal, ErrorCategory::Internal, message, loc));
}

// EnhancedErrorManager 实现
EnhancedErrorManager::EnhancedErrorManager() {
    reporter_ = std::make_shared<EnhancedErrorReporter>();
    recovery_ = std::make_shared<ErrorRecovery>();
    analyzer_ = std::make_shared<ErrorAnalyzer>();
}

} // namespace error
} // namespace chtl