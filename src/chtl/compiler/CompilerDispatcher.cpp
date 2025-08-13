#include "CompilerDispatcher.h"
#include "handwritten/CHTLCompilerWrapper.h"
#include "handwritten/CHTLJSCompilerWrapper.h"
#include "antlr/CSSCompilerWrapper.h"
#include "antlr/JSCompilerWrapper.h"
#include "../error/ErrorInterface.h"
#include <sstream>

namespace chtl {
namespace compiler {

CompilerDispatcher::CompilerDispatcher() 
    : errorReporter_(error::ErrorManager::getInstance().getReporter()) {
}

void CompilerDispatcher::registerCompilerFactory(
    scanner::FragmentType type, 
    std::unique_ptr<ICompilerFactory> factory) {
    factories_[type] = std::move(factory);
}

CompileResult CompilerDispatcher::compileFragment(const scanner::CodeFragment& fragment) {
    // 获取对应的编译器
    auto compiler = getCompiler(fragment.type);
    if (!compiler) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("No compiler registered for fragment type: " + 
                               std::to_string(static_cast<int>(fragment.type)));
        return result;
    }
    
    // 设置错误报告器
    compiler->setErrorReporter(errorReporter_);
    
    // 编译片段
    return compiler->compile(fragment);
}

CompileResult CompilerDispatcher::compileFragments(
    const std::vector<scanner::CodeFragment>& fragments) {
    
    std::vector<CompileResult> results;
    
    // 编译每个片段
    for (const auto& fragment : fragments) {
        results.push_back(compileFragment(fragment));
    }
    
    // 合并结果
    return mergeResults(results);
}

void CompilerDispatcher::setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter) {
    errorReporter_ = reporter;
}

std::unique_ptr<ICompiler> CompilerDispatcher::getCompiler(scanner::FragmentType type) {
    // 检查缓存
    auto cacheIt = compilers_.find(type);
    if (cacheIt != compilers_.end() && cacheIt->second) {
        return std::move(cacheIt->second);
    }
    
    // 使用工厂创建新编译器
    auto factoryIt = factories_.find(type);
    if (factoryIt != factories_.end() && factoryIt->second) {
        auto compiler = factoryIt->second->createCompiler();
        compiler->setErrorReporter(errorReporter_);
        return compiler;
    }
    
    return nullptr;
}

CompileResult CompilerDispatcher::mergeResults(const std::vector<CompileResult>& results) {
    CompileResult merged;
    merged.success = true;
    
    std::stringstream output;
    
    for (const auto& result : results) {
        if (!result.success) {
            merged.success = false;
        }
        
        // 合并输出
        output << result.output;
        
        // 合并错误
        merged.errors.insert(merged.errors.end(), 
                           result.errors.begin(), 
                           result.errors.end());
        
        // 合并警告
        merged.warnings.insert(merged.warnings.end(), 
                             result.warnings.begin(), 
                             result.warnings.end());
    }
    
    merged.output = output.str();
    return merged;
}

std::unique_ptr<CompilerDispatcher> createStandardDispatcher(
    std::shared_ptr<CHTLContext> context) {
    
    auto dispatcher = std::make_unique<CompilerDispatcher>();
    
    // 注册手写编译器
    dispatcher->registerCompilerFactory(
        scanner::FragmentType::CHTL,
        std::make_unique<handwritten::CHTLCompilerFactory>()
    );
    
    dispatcher->registerCompilerFactory(
        scanner::FragmentType::CHTL_JS,
        std::make_unique<handwritten::CHTLJSCompilerFactory>()
    );
    
    // 注册ANTLR编译器
    dispatcher->registerCompilerFactory(
        scanner::FragmentType::CSS,
        std::make_unique<antlr::CSSCompilerFactory>(context)
    );
    
    dispatcher->registerCompilerFactory(
        scanner::FragmentType::JS,
        std::make_unique<antlr::JSCompilerFactory>(context)
    );
    
    return dispatcher;
}

} // namespace compiler
} // namespace chtl