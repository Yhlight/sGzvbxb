#ifndef CSS_COMPILER_WRAPPER_H
#define CSS_COMPILER_WRAPPER_H

#include "../CompilerInterface.h"
#include "../../CHTLCSSCompiler.h"
#include <memory>

namespace chtl {
namespace compiler {
namespace antlr {

/**
 * CSS ANTLR编译器包装器
 * 使用ANTLR生成的CSS解析器
 */
class CSSCompilerWrapper : public ICompiler {
public:
    CSSCompilerWrapper(std::shared_ptr<CHTLContext> context);
    ~CSSCompilerWrapper() override = default;
    
    CompileResult compile(const scanner::CodeFragment& fragment) override;
    void setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter) override;
    scanner::FragmentType getSupportedType() const override { 
        return scanner::FragmentType::CSS; 
    }
    
    // CSS特定的编译选项
    void setMinify(bool minify) { cssCompiler_->setMinify(minify); }
    void setPreserveComments(bool preserve) { cssCompiler_->setPreserveComments(preserve); }
    
private:
    std::unique_ptr<CSSCompiler> cssCompiler_;
    std::shared_ptr<error::IErrorReporter> errorReporter_;
};

/**
 * CSS编译器工厂
 */
class CSSCompilerFactory : public ICompilerFactory {
private:
    std::shared_ptr<CHTLContext> context_;
    
public:
    explicit CSSCompilerFactory(std::shared_ptr<CHTLContext> context) 
        : context_(context) {}
    
    std::unique_ptr<ICompiler> createCompiler() override {
        return std::make_unique<CSSCompilerWrapper>(context_);
    }
};

} // namespace antlr
} // namespace compiler
} // namespace chtl

#endif // CSS_COMPILER_WRAPPER_H