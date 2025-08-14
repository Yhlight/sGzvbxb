#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include "../scanner/CHTLUnifiedScanner.h"
#include "../parser/standalone/CHTLParserEnhanced.h"
#include "../parser/standalone/CHTLCodeGenVisitor.h"

namespace chtl {
namespace compiler {

// 编译结果
struct CompilationResult {
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    bool success;
};

// 片段编译结果
struct FragmentResult {
    scanner::FragmentType type;
    std::string output;
    std::vector<std::string> errors;
};

// 编译器调度器 - 协调各个编译器
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 编译源代码
    CompilationResult compile(const std::string& source);
    
    // 配置选项
    void setDebugMode(bool debug) { debugMode_ = debug; }
    void setOptimizationLevel(int level) { optimizationLevel_ = level; }
    
private:
    // 成员变量
    std::unique_ptr<scanner::CHTLUnifiedScanner> scanner_;
    std::unique_ptr<parser::CHTLParserEnhanced> chtlParser_;
    std::unique_ptr<parser::CHTLCodeGenVisitor> codeGenerator_;
    
    bool debugMode_;
    int optimizationLevel_;
    
    // 编译方法
    FragmentResult compileCHTLFragment(const scanner::CodeFragment& fragment);
    FragmentResult compileCHTLJSFragment(const scanner::CodeFragment& fragment);
    FragmentResult compileCSSFragment(const scanner::CodeFragment& fragment);
    FragmentResult compileJavaScriptFragment(const scanner::CodeFragment& fragment);
    
    // 结果合并
    CompilationResult mergeResults(const std::vector<FragmentResult>& results);
    
    // 辅助方法
    std::string processTemplate(const std::string& content);
    std::string processCHTLJS(const std::string& content);
    std::string optimizeCSS(const std::string& css);
    std::string optimizeJS(const std::string& js);
};

} // namespace compiler
} // namespace chtl