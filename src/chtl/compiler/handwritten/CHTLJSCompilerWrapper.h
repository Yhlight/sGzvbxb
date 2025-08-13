#ifndef CHTLJS_COMPILER_WRAPPER_H
#define CHTLJS_COMPILER_WRAPPER_H

#include "../CompilerInterface.h"
#include "../../parser/standalone/CHTLJSParser.h"
#include <memory>

namespace chtl {
namespace compiler {
namespace handwritten {

/**
 * CHTL JS手写编译器包装器
 * 使用手写的CHTL JS解析器
 */
class CHTLJSCompilerWrapper : public ICompiler {
public:
    CHTLJSCompilerWrapper();
    ~CHTLJSCompilerWrapper() override = default;
    
    CompileResult compile(const scanner::CodeFragment& fragment) override;
    void setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter) override;
    scanner::FragmentType getSupportedType() const override { 
        return scanner::FragmentType::CHTL_JS; 
    }
    
private:
    std::shared_ptr<error::IErrorReporter> errorReporter_;
};

/**
 * CHTL JS编译器工厂
 */
class CHTLJSCompilerFactory : public ICompilerFactory {
public:
    std::unique_ptr<ICompiler> createCompiler() override {
        return std::make_unique<CHTLJSCompilerWrapper>();
    }
};

} // namespace handwritten
} // namespace compiler
} // namespace chtl

#endif // CHTLJS_COMPILER_WRAPPER_H