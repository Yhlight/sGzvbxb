#ifndef CHTL_COMPILER_WRAPPER_H
#define CHTL_COMPILER_WRAPPER_H

#include "../CompilerInterface.h"
#include "../../parser/standalone/CHTLParser.h"
#include "../../generator/HtmlGenerator.h"
#include <memory>

namespace chtl {
namespace compiler {
namespace handwritten {

/**
 * CHTL手写编译器包装器
 * 使用手写的CHTL解析器和HTML生成器
 */
class CHTLCompilerWrapper : public ICompiler {
public:
    CHTLCompilerWrapper();
    ~CHTLCompilerWrapper() override = default;
    
    CompileResult compile(const scanner::CodeFragment& fragment) override;
    void setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter) override;
    scanner::FragmentType getSupportedType() const override { 
        return scanner::FragmentType::CHTL; 
    }
    
private:
    std::shared_ptr<error::IErrorReporter> errorReporter_;
    std::unique_ptr<generator::HtmlGenerator> htmlGenerator_;
};

/**
 * CHTL编译器工厂
 */
class CHTLCompilerFactory : public ICompilerFactory {
public:
    std::unique_ptr<ICompiler> createCompiler() override {
        return std::make_unique<CHTLCompilerWrapper>();
    }
};

} // namespace handwritten
} // namespace compiler
} // namespace chtl

#endif // CHTL_COMPILER_WRAPPER_H