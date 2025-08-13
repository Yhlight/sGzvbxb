#ifndef CHTL_ERROR_COLLECTOR_H
#define CHTL_ERROR_COLLECTOR_H

#include "ErrorInterface.h"
#include <vector>
#include <string>
#include <sstream>

namespace chtl {
namespace error {

/**
 * 错误收集器 - 将错误收集到字符串向量中
 * 用于需要收集错误而不是直接报告的场景
 */
class ErrorCollector : public IErrorReporter {
public:
    explicit ErrorCollector(std::vector<std::string>& errorList) 
        : errors(errorList) {}
    
    // 实现IErrorReporter接口
    void report(const ErrorInfo& error) override {
        std::stringstream ss;
        if (!error.location.filename.empty()) {
            ss << error.location.toString() << ": ";
        }
        
        switch (error.level) {
            case ErrorLevel::Debug:
                ss << "[DEBUG] ";
                break;
            case ErrorLevel::Info:
                ss << "[INFO] ";
                break;
            case ErrorLevel::Warning:
                ss << "[WARNING] ";
                break;
            case ErrorLevel::Error:
                ss << "[ERROR] ";
                break;
            case ErrorLevel::Fatal:
                ss << "[FATAL] ";
                break;
        }
        
        ss << error.message;
        errors.push_back(ss.str());
    }
    
    void debug(const std::string& message, const SourceLocation& loc) override {
        ErrorInfo info;
        info.level = ErrorLevel::Debug;
        info.category = ErrorCategory::Unknown;
        info.message = message;
        info.location = loc;
        report(info);
    }
    
    void info(const std::string& message, const SourceLocation& loc) override {
        ErrorInfo info;
        info.level = ErrorLevel::Info;
        info.category = ErrorCategory::Unknown;
        info.message = message;
        info.location = loc;
        report(info);
    }
    
    void warning(const std::string& message, const SourceLocation& loc) override {
        ErrorInfo info;
        info.level = ErrorLevel::Warning;
        info.category = ErrorCategory::Unknown;
        info.message = message;
        info.location = loc;
        report(info);
    }
    
    void error(const std::string& message, const SourceLocation& loc) override {
        ErrorInfo info;
        info.level = ErrorLevel::Error;
        info.category = ErrorCategory::Syntax;
        info.message = message;
        info.location = loc;
        report(info);
    }
    
    void fatal(const std::string& message, const SourceLocation& loc) override {
        ErrorInfo info;
        info.level = ErrorLevel::Fatal;
        info.category = ErrorCategory::Syntax;
        info.message = message;
        info.location = loc;
        report(info);
    }
    
    // 获取错误数量
    size_t getErrorCount() const {
        return errors.size();
    }
    
    // 清空错误
    void clear() {
        errors.clear();
    }
    
private:
    std::vector<std::string>& errors;
};

} // namespace error
} // namespace chtl

#endif // CHTL_ERROR_COLLECTOR_H