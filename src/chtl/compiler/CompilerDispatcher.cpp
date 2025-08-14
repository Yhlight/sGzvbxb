#include "CompilerDispatcher.h"
#include "../parser/standalone/CHTLLexer.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace chtl {
namespace compiler {

CompilerDispatcher::CompilerDispatcher()
    : scanner_(std::make_unique<scanner::CHTLUnifiedScanner>())
    , debugMode_(false)
    , optimizationLevel_(0) {
}

CompilerDispatcher::~CompilerDispatcher() = default;

CompilationResult CompilerDispatcher::compile(const std::string& source) {
    CompilationResult result;
    result.success = true;
    
    try {
        // 步骤1：扫描源代码，切割成片段
        if (debugMode_) {
            std::cout << "=== Scanner Phase ===" << std::endl;
            scanner_->setDebugMode(true);
        }
        
        auto fragments = scanner_->scan(source);
        
        if (debugMode_) {
            std::cout << "Found " << fragments.size() << " fragments" << std::endl;
        }
        
        // 步骤2：编译每个片段
        std::vector<FragmentResult> fragmentResults;
        
        for (const auto& fragment : fragments) {
            FragmentResult fragResult;
            
            switch (fragment.type) {
                case scanner::FragmentType::CHTL:
                    fragResult = compileCHTLFragment(fragment);
                    break;
                    
                case scanner::FragmentType::CHTL_JS:
                    fragResult = compileCHTLJSFragment(fragment);
                    break;
                    
                case scanner::FragmentType::CSS:
                    fragResult = compileCSSFragment(fragment);
                    break;
                    
                case scanner::FragmentType::JAVASCRIPT:
                    fragResult = compileJavaScriptFragment(fragment);
                    break;
            }
            
            // 收集错误
            if (!fragResult.errors.empty()) {
                result.errors.insert(result.errors.end(),
                                   fragResult.errors.begin(),
                                   fragResult.errors.end());
                result.success = false;
            }
            
            fragmentResults.push_back(fragResult);
        }
        
        // 步骤3：合并结果
        result = mergeResults(fragmentResults);
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("Compilation error: ") + e.what());
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileCHTLFragment(const scanner::CodeFragment& fragment) {
    FragmentResult result;
    result.type = fragment.type;
    
    try {
        // 使用增强解析器编译 CHTL
        parser::CHTLLexer lexer(fragment.content);
        auto tokens = lexer.tokenize();
        auto tokenStream = std::make_shared<parser::TokenStream>(tokens);
        
        parser::CHTLParserEnhanced parser(tokenStream);
        auto parseTree = parser.compilationUnit();
        
        if (!parseTree) {
            result.errors.push_back("Failed to parse CHTL fragment");
            return result;
        }
        
        // 使用代码生成器
        parser::CHTLCodeGenVisitor generator;
        result.output = generator.visit(parseTree);
        
    } catch (const std::exception& e) {
        result.errors.push_back(std::string("CHTL compilation error: ") + e.what());
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileCHTLJSFragment(const scanner::CodeFragment& fragment) {
    FragmentResult result;
    result.type = fragment.type;
    
    try {
        // 处理 CHTL JS 特性
        result.output = processCHTLJS(fragment.content);
        
        // 如果需要，进行优化
        if (optimizationLevel_ > 0) {
            result.output = optimizeJS(result.output);
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back(std::string("CHTL JS compilation error: ") + e.what());
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileCSSFragment(const scanner::CodeFragment& fragment) {
    FragmentResult result;
    result.type = fragment.type;
    
    try {
        // 暂时直接输出，后续集成 ANTLR CSS 解析器
        result.output = fragment.content;
        
        // 如果需要，进行优化
        if (optimizationLevel_ > 0) {
            result.output = optimizeCSS(result.output);
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back(std::string("CSS compilation error: ") + e.what());
    }
    
    return result;
}

FragmentResult CompilerDispatcher::compileJavaScriptFragment(const scanner::CodeFragment& fragment) {
    FragmentResult result;
    result.type = fragment.type;
    
    try {
        // 暂时直接输出，后续集成 ANTLR JavaScript 解析器
        result.output = fragment.content;
        
        // 如果需要，进行优化
        if (optimizationLevel_ > 0) {
            result.output = optimizeJS(result.output);
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back(std::string("JavaScript compilation error: ") + e.what());
    }
    
    return result;
}

CompilationResult CompilerDispatcher::mergeResults(const std::vector<FragmentResult>& results) {
    CompilationResult merged;
    merged.success = true;
    
    std::stringstream htmlStream;
    std::stringstream cssStream;
    std::stringstream jsStream;
    
    // 生成 HTML 文档结构
    htmlStream << "<!DOCTYPE html>\n";
    htmlStream << "<html>\n";
    htmlStream << "<head>\n";
    htmlStream << "    <meta charset=\"UTF-8\">\n";
    htmlStream << "    <title>CHTL Generated Page</title>\n";
    
    // 收集所有片段
    for (const auto& result : results) {
        switch (result.type) {
            case scanner::FragmentType::CHTL:
                // CHTL 已经生成完整的 HTML，提取 body 内容
                {
                    // 简单的字符串查找替代regex（避免C++17兼容性问题）
                    size_t bodyStart = result.output.find("<body>");
                    size_t bodyEnd = result.output.find("</body>");
                    if (bodyStart != std::string::npos && bodyEnd != std::string::npos) {
                        bodyStart += 6; // length of "<body>"
                        htmlStream << result.output.substr(bodyStart, bodyEnd - bodyStart);
                    }
                    
                    // 提取 style
                    size_t styleStart = result.output.find("<style>");
                    size_t styleEnd = result.output.find("</style>");
                    if (styleStart != std::string::npos && styleEnd != std::string::npos) {
                        styleStart += 7; // length of "<style>"
                        cssStream << result.output.substr(styleStart, styleEnd - styleStart);
                    }
                    
                    // 提取 script
                    size_t scriptStart = result.output.find("<script>");
                    size_t scriptEnd = result.output.find("</script>");
                    if (scriptStart != std::string::npos && scriptEnd != std::string::npos) {
                        scriptStart += 8; // length of "<script>"
                        jsStream << result.output.substr(scriptStart, scriptEnd - scriptStart);
                    }
                }
                break;
                
            case scanner::FragmentType::CSS:
                cssStream << result.output << "\n";
                break;
                
            case scanner::FragmentType::CHTL_JS:
            case scanner::FragmentType::JAVASCRIPT:
                jsStream << result.output << "\n";
                break;
        }
        
        // 收集错误
        if (!result.errors.empty()) {
            merged.errors.insert(merged.errors.end(),
                               result.errors.begin(),
                               result.errors.end());
            merged.success = false;
        }
    }
    
    // 添加 CSS
    if (!cssStream.str().empty()) {
        htmlStream << "    <style>\n";
        htmlStream << cssStream.str();
        htmlStream << "    </style>\n";
    }
    
    htmlStream << "</head>\n";
    htmlStream << "<body>\n";
    
    // body 内容已经在上面添加了
    
    htmlStream << "</body>\n";
    
    // 添加 JavaScript
    if (!jsStream.str().empty()) {
        htmlStream << "<script>\n";
        htmlStream << jsStream.str();
        htmlStream << "</script>\n";
    }
    
    htmlStream << "</html>\n";
    
    merged.html = htmlStream.str();
    merged.css = cssStream.str();
    merged.javascript = jsStream.str();
    
    return merged;
}

std::string CompilerDispatcher::processCHTLJS(const std::string& content) {
    std::string result = content;
    
    // 转换 {{selector}} 语法
    std::regex selectorRegex(R"(\{\{([^\}]+)\}\})");
    result = std::regex_replace(result, selectorRegex, "CHTL.select('$1')");
    
    // 转换 -> 为 .
    size_t pos = 0;
    while ((pos = result.find("->", pos)) != std::string::npos) {
        result.replace(pos, 2, ".");
        pos += 1;
    }
    
    return result;
}

std::string CompilerDispatcher::optimizeCSS(const std::string& css) {
    // 简单的 CSS 优化：移除多余空白
    std::string optimized = css;
    
    // 移除注释
    std::regex commentRegex(R"(/\*.*?\*/)");
    optimized = std::regex_replace(optimized, commentRegex, "");
    
    // 压缩空白
    std::regex whitespaceRegex(R"(\s+)");
    optimized = std::regex_replace(optimized, whitespaceRegex, " ");
    
    // 移除分号前的空格
    std::regex semiSpaceRegex(R"(\s*;\s*)");
    optimized = std::regex_replace(optimized, semiSpaceRegex, ";");
    
    // 移除花括号周围的空格
    std::regex braceSpaceRegex(R"(\s*\{\s*)");
    optimized = std::regex_replace(optimized, braceSpaceRegex, "{");
    
    std::regex braceSpaceRegex2(R"(\s*\}\s*)");
    optimized = std::regex_replace(optimized, braceSpaceRegex2, "}");
    
    return optimized;
}

std::string CompilerDispatcher::optimizeJS(const std::string& js) {
    // 简单的 JS 优化：移除注释和多余空白
    std::string optimized = js;
    
    // 移除单行注释
    std::regex singleLineCommentRegex(R"(//.*$)", std::regex::multiline);
    optimized = std::regex_replace(optimized, singleLineCommentRegex, "");
    
    // 移除多行注释
    std::regex multiLineCommentRegex(R"(/\*.*?\*/)");
    optimized = std::regex_replace(optimized, multiLineCommentRegex, "");
    
    // 压缩多余空白
    std::regex whitespaceRegex(R"(\s+)");
    optimized = std::regex_replace(optimized, whitespaceRegex, " ");
    
    return optimized;
}

} // namespace compiler
} // namespace chtl