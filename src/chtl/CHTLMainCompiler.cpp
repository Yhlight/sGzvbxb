#include "CHTLMainCompiler.h"
#include "compiler/antlr/CSSCompilerWrapper.h"
#include "compiler/antlr/JSCompilerWrapper.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace chtl {

CHTLMainCompiler::CHTLMainCompiler() {
    initialize();
}

void CHTLMainCompiler::initialize() {
    // 创建上下文
    context_ = std::make_shared<CHTLContext>();
    
    // 创建精确扫描器
    scanner_ = std::make_unique<scanner::PrecisionScanner>();
    
    // 创建调度器并注册编译器
    dispatcher_ = compiler::createStandardDispatcher(context_);
}

bool CHTLMainCompiler::compileFile(const std::string& sourceFile, const std::string& outputFile) {
    // 读取源文件
    std::ifstream input(sourceFile);
    if (!input.is_open()) {
        errors_.push_back("Cannot open source file: " + sourceFile);
        return false;
    }
    
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string source = buffer.str();
    input.close();
    
    // 设置当前文件
    context_->setCurrentFile(sourceFile);
    
    // 编译
    auto result = doCompile(source);
    
    if (!result.success) {
        return false;
    }
    
    // 写入输出文件
    std::ofstream output(outputFile);
    if (!output.is_open()) {
        errors_.push_back("Cannot create output file: " + outputFile);
        return false;
    }
    
    output << result.output;
    output.close();
    
    return true;
}

std::string CHTLMainCompiler::compileString(const std::string& source) {
    auto result = doCompile(source);
    return result.output;
}

compiler::CompileResult CHTLMainCompiler::doCompile(const std::string& source) {
    compiler::CompileResult result;
    errors_.clear();
    warnings_.clear();
    
    try {
        // 第一步：扫描器切割代码
        auto fragments = scanner_->scan(source);
        
        if (scanner_->hasErrors()) {
            result.success = false;
            for (const auto& error : scanner_->getErrors()) {
                result.errors.push_back(error);
                errors_.push_back(error);
            }
            return result;
        }
        
        // 第二步：调度器分发编译
        result = dispatcher_->compileFragments(fragments);
        
        // 收集错误和警告
        errors_ = result.errors;
        warnings_ = result.warnings;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("Compilation error: ") + e.what());
        errors_.push_back(std::string("Compilation error: ") + e.what());
    }
    
    return result;
}

void CHTLMainCompiler::setMinifyCSS(bool minify) {
    // 获取CSS编译器并设置选项
    auto cssCompiler = dispatcher_->getCompiler(scanner::FragmentType::CSS);
    if (cssCompiler) {
        // 使用dynamic_cast安全地向下转型到CSSCompilerWrapper
        auto cssWrapper = dynamic_cast<compiler::antlr::CSSCompilerWrapper*>(cssCompiler.get());
        if (cssWrapper) {
            cssWrapper->setMinify(minify);
        } else {
            // 如果不是预期的类型，记录警告
            std::cerr << "警告: CSS编译器不是CSSCompilerWrapper类型，无法设置minify选项" << std::endl;
        }
    }
}

void CHTLMainCompiler::setMinifyJS(bool minify) {
    // 获取JS编译器并设置选项
    auto jsCompiler = dispatcher_->getCompiler(scanner::FragmentType::JS);
    if (jsCompiler) {
        // 使用dynamic_cast安全地向下转型到JSCompilerWrapper
        auto jsWrapper = dynamic_cast<compiler::antlr::JSCompilerWrapper*>(jsCompiler.get());
        if (jsWrapper) {
            jsWrapper->setMinify(minify);
        } else {
            // 如果不是预期的类型，记录警告
            std::cerr << "警告: JS编译器不是JSCompilerWrapper类型，无法设置minify选项" << std::endl;
        }
    }
}

void CHTLMainCompiler::setStrictMode(bool strict) {
    // 设置严格模式
    context_->setConfigValue("strict_mode", strict);
}

} // namespace chtl