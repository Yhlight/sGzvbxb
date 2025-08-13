#include "ErrorInterface.h"
#include <sstream>
#include <iostream>

namespace chtl {
namespace error {

// SourceLocation实现
std::string SourceLocation::toString() const {
    std::ostringstream oss;
    if (!filename.empty()) {
        oss << filename << ":";
    }
    oss << line << ":" << column;
    return oss.str();
}

// 默认错误处理器实现
class DefaultErrorHandler : public IErrorHandler {
private:
    ErrorLevel minLevel = ErrorLevel::Debug;
    size_t errorCount = 0;
    size_t warningCount = 0;
    std::vector<ErrorInfo> errors;
    
public:
    void handle(const ErrorInfo& error) override {
        if (error.level >= minLevel) {
            errors.push_back(error);
            
            if (error.level == ErrorLevel::Error || error.level == ErrorLevel::Fatal) {
                errorCount++;
            } else if (error.level == ErrorLevel::Warning) {
                warningCount++;
            }
            
            // 输出到stderr
            std::cerr << "[" << getLevelString(error.level) << "] ";
            if (!error.location.filename.empty()) {
                std::cerr << error.location.toString() << ": ";
            }
            std::cerr << error.message << std::endl;
            
            if (!error.detail.empty()) {
                std::cerr << "  " << error.detail << std::endl;
            }
        }
    }
    
    void setMinLevel(ErrorLevel level) override {
        minLevel = level;
    }
    
    size_t getErrorCount() const override { return errorCount; }
    size_t getWarningCount() const override { return warningCount; }
    
    bool hasErrors() const override { return errorCount > 0; }
    bool hasWarnings() const override { return warningCount > 0; }
    
    void clear() override {
        errors.clear();
        errorCount = 0;
        warningCount = 0;
    }
    
private:
    const char* getLevelString(ErrorLevel level) {
        switch (level) {
            case ErrorLevel::Debug: return "DEBUG";
            case ErrorLevel::Info: return "INFO";
            case ErrorLevel::Warning: return "WARNING";
            case ErrorLevel::Error: return "ERROR";
            case ErrorLevel::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
};

// 默认错误报告器实现
class DefaultErrorReporter : public IErrorReporter {
private:
    std::shared_ptr<IErrorHandler> handler;
    bool throwOnError;
    
public:
    DefaultErrorReporter(std::shared_ptr<IErrorHandler> h, bool toe) 
        : handler(h), throwOnError(toe) {}
    
    void report(const ErrorInfo& error) override {
        if (handler) {
            handler->handle(error);
        }
        
        if (throwOnError && (error.level == ErrorLevel::Error || error.level == ErrorLevel::Fatal)) {
            throw CHTLException(error);
        }
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
        info.category = ErrorCategory::Unknown;
        info.message = message;
        info.location = loc;
        report(info);
    }
    
    void fatal(const std::string& message, const SourceLocation& loc) override {
        ErrorInfo info;
        info.level = ErrorLevel::Fatal;
        info.category = ErrorCategory::Unknown;
        info.message = message;
        info.location = loc;
        report(info);
    }
};

// ErrorManager单例实现
ErrorManager& ErrorManager::getInstance() {
    static ErrorManager instance;
    return instance;
}

void ErrorManager::setHandler(std::shared_ptr<IErrorHandler> handler) {
    handler_ = handler;
    reporter_ = std::make_shared<DefaultErrorReporter>(handler_, throwOnError_);
}

std::shared_ptr<IErrorReporter> ErrorManager::getReporter() {
    if (!reporter_) {
        if (!handler_) {
            handler_ = std::make_shared<DefaultErrorHandler>();
        }
        reporter_ = std::make_shared<DefaultErrorReporter>(handler_, throwOnError_);
    }
    return reporter_;
}

} // namespace error
} // namespace chtl