#ifndef CHTL_ANTLR_ERROR_ADAPTER_H
#define CHTL_ANTLR_ERROR_ADAPTER_H

#include "ErrorInterface.h"
#include "antlr4-runtime.h"
#include <memory>

namespace chtl {
namespace error {

/**
 * ANTLR错误监听器适配器
 * 将ANTLR的错误报告机制适配到CHTL的ErrorInterface系统
 */
class ANTLRErrorAdapter : public antlr4::BaseErrorListener {
public:
    explicit ANTLRErrorAdapter(std::shared_ptr<IErrorReporter> reporter)
        : errorReporter(reporter) {}
    
    // 覆盖ANTLR的语法错误处理
    void syntaxError(
        antlr4::Recognizer* recognizer,
        antlr4::Token* offendingSymbol,
        size_t line,
        size_t charPositionInLine,
        const std::string& msg,
        std::exception_ptr e) override {
        
        // 创建源位置信息
        SourceLocation location;
        location.filename = currentFile;
        location.line = static_cast<int>(line);
        location.column = static_cast<int>(charPositionInLine);
        
        // 报告错误
        if (errorReporter) {
            errorReporter->error("Syntax error: " + msg, location);
        }
    }
    
    // 设置当前文件名
    void setCurrentFile(const std::string& filename) {
        currentFile = filename;
    }
    
    // 获取错误报告器
    std::shared_ptr<IErrorReporter> getErrorReporter() const {
        return errorReporter;
    }
    
private:
    std::shared_ptr<IErrorReporter> errorReporter;
    std::string currentFile;
};

/**
 * 创建ANTLR错误适配器的便捷函数
 */
inline std::unique_ptr<ANTLRErrorAdapter> createANTLRErrorAdapter(
    std::shared_ptr<IErrorReporter> reporter = nullptr) {
    
    if (!reporter) {
        // 如果没有提供报告器，使用默认的错误管理器
        reporter = ErrorManager::getInstance().getReporter();
    }
    
    return std::make_unique<ANTLRErrorAdapter>(reporter);
}

} // namespace error
} // namespace chtl

#endif // CHTL_ANTLR_ERROR_ADAPTER_H