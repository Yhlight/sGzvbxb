#ifndef JS_COMPILER_WRAPPER_H
#define JS_COMPILER_WRAPPER_H

#include "../CompilerInterface.h"
#include "../../CHTLJSCompiler.h"
#include <memory>

namespace chtl {
namespace compiler {
namespace antlr {

/**
 * JavaScript ANTLR编译器包装器
 * 使用ANTLR生成的JavaScript解析器
 */
class JSCompilerWrapper : public ICompiler {
public:
    JSCompilerWrapper(std::shared_ptr<CHTLContext> context);
    ~JSCompilerWrapper() override = default;
    
    CompileResult compile(const scanner::CodeFragment& fragment) override;
    void setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter) override;
    scanner::FragmentType getSupportedType() const override { 
        return scanner::FragmentType::JAVASCRIPT; 
    }
    
    // JS特定的编译选项
    void setMinify(bool minify) { jsCompiler_->setMinify(minify); }
    void setStrictMode(bool strict) { jsCompiler_->setStrictMode(strict); }
    void setModernize(bool modernize) { jsCompiler_->setModernize(modernize); }
    
private:
    std::unique_ptr<JSCompiler> jsCompiler_;
    std::shared_ptr<error::IErrorReporter> errorReporter_;
};

/**
 * JavaScript编译器工厂
 */
class JSCompilerFactory : public ICompilerFactory {
private:
    std::shared_ptr<CHTLContext> context_;
    
public:
    explicit JSCompilerFactory(std::shared_ptr<CHTLContext> context) 
        : context_(context) {}
    
    std::unique_ptr<ICompiler> createCompiler() override {
        return std::make_unique<JSCompilerWrapper>(context_);
    }
};

} // namespace antlr
} // namespace compiler
} // namespace chtl

#endif // JS_COMPILER_WRAPPER_H