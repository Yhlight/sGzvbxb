#include "CHTLJSCompilerWrapper.h"
#include "../../parser/standalone/CHTLJSLexer.h"
#include "../../parser/standalone/CHTLJSCodeGenerator.h"
#include "../../error/ErrorCollector.h"
#include <sstream>

namespace chtl {
namespace compiler {
namespace handwritten {

CHTLJSCompilerWrapper::CHTLJSCompilerWrapper() 
    : errorReporter_(error::ErrorManager::getInstance().getReporter()) {
}

CompileResult CHTLJSCompilerWrapper::compile(const scanner::CodeFragment& fragment) {
    CompileResult result;
    
    // 只处理CHTL_JS片段
    if (fragment.type != scanner::FragmentType::CHTL_JS) {
        result.success = false;
        result.errors.push_back("CHTLJSCompiler can only compile CHTL JS fragments");
        return result;
    }
    
    try {
        // 创建词法分析器
        parser::CHTLJSLexer lexer(fragment.content);
        auto tokens = lexer.tokenize();
        
        // 创建token流
        auto tokenStream = std::make_shared<parser::TokenStream>(tokens);
        
        // 创建解析器
        parser::CHTLJSParser jsParser(tokenStream);
        auto parseTree = jsParser.parse();
        
        if (!parseTree) {
            result.success = false;
            result.errors.push_back("Failed to parse CHTL JS code");
            return result;
        }
        
        // 使用代码生成器生成JavaScript
        parser::CHTLJSCodeGenerator generator;
        result.output = generator.generate(parseTree);
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("CHTL JS compilation error: ") + e.what());
        
        if (errorReporter_) {
            errorReporter_->error(e.what());
        }
    }
    
    return result;
}

void CHTLJSCompilerWrapper::setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter) {
    errorReporter_ = reporter;
}

} // namespace handwritten
} // namespace compiler
} // namespace chtl