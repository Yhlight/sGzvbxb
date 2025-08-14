#ifndef CHTL_COMPILER_INTERFACE_H
#define CHTL_COMPILER_INTERFACE_H

#include <string>
#include <memory>
#include <vector>
#include "../scanner/CHTLUnifiedScanner.h"
#include "../error/ErrorInterface.h"

namespace chtl {
namespace compiler {

/**
 * 编译结果
 */
struct CompileResult {
    bool success;
    std::string output;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

/**
 * 通用编译器接口
 * 所有编译器（手写或ANTLR生成）都必须实现此接口
 */
class ICompiler {
public:
    virtual ~ICompiler() = default;
    
    /**
     * 编译代码片段
     * @param fragment 要编译的代码片段
     * @return 编译结果
     */
    virtual CompileResult compile(const scanner::CodeFragment& fragment) = 0;
    
    /**
     * 设置错误报告器
     */
    virtual void setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter) = 0;
    
    /**
     * 获取支持的片段类型
     */
    virtual scanner::FragmentType getSupportedType() const = 0;
};

/**
 * 编译器工厂接口
 */
class ICompilerFactory {
public:
    virtual ~ICompilerFactory() = default;
    
    /**
     * 创建编译器实例
     */
    virtual std::unique_ptr<ICompiler> createCompiler() = 0;
};

} // namespace compiler
} // namespace chtl

#endif // CHTL_COMPILER_INTERFACE_H