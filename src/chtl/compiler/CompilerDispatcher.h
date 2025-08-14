#ifndef CHTL_COMPILER_DISPATCHER_H
#define CHTL_COMPILER_DISPATCHER_H

#include "CompilerInterface.h"
#include "../scanner/CHTLUnifiedScanner.h"
#include <unordered_map>
#include <memory>
#include <vector>

namespace chtl {

// 前向声明
class CHTLContext;

namespace compiler {

/**
 * 编译器调度器
 * 负责将扫描器切割的代码片段分发到对应的编译器
 * 
 * 架构设计：
 * - CHTL编译器：手写实现
 * - CHTL JS编译器：手写实现
 * - CSS编译器：ANTLR生成
 * - JavaScript编译器：ANTLR生成
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher() = default;
    
    /**
     * 注册编译器工厂
     */
    void registerCompilerFactory(
        scanner::FragmentType type, 
        std::unique_ptr<ICompilerFactory> factory
    );
    
    /**
     * 编译单个片段
     */
    CompileResult compileFragment(const scanner::CodeFragment& fragment);
    
    /**
     * 编译所有片段并合并结果
     */
    CompileResult compileFragments(const std::vector<scanner::CodeFragment>& fragments);
    
    /**
     * 设置全局错误报告器
     */
    void setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter);
    
    /**
     * 获取特定类型的编译器
     */
    std::unique_ptr<ICompiler> getCompiler(scanner::FragmentType type);
    
private:
    // 编译器工厂映射
    std::unordered_map<scanner::FragmentType, std::unique_ptr<ICompilerFactory>> factories_;
    
    // 编译器缓存（可选）
    std::unordered_map<scanner::FragmentType, std::unique_ptr<ICompiler>> compilers_;
    
    // 全局错误报告器
    std::shared_ptr<error::IErrorReporter> errorReporter_;
    
    // 合并多个编译结果
    CompileResult mergeResults(const std::vector<CompileResult>& results);
};

/**
 * 创建预配置的编译器调度器
 * 自动注册所有标准编译器
 */
std::unique_ptr<CompilerDispatcher> createStandardDispatcher(
    std::shared_ptr<CHTLContext> context
);

} // namespace compiler
} // namespace chtl

#endif // CHTL_COMPILER_DISPATCHER_H