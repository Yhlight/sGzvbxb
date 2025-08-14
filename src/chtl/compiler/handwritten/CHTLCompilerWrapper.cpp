#include "CHTLCompilerWrapper.h"
#include "../../parser/standalone/CHTLLexer.h"
#include "../../parser/CHTLSimpleParser.h"
#include "../../error/ErrorCollector.h"
#include <sstream>

namespace chtl {
namespace compiler {
namespace handwritten {

CHTLCompilerWrapper::CHTLCompilerWrapper() 
    : errorReporter_(error::ErrorManager::getInstance().getReporter()),
      htmlGenerator_(std::make_unique<generator::HtmlGenerator>()) {
}

CompileResult CHTLCompilerWrapper::compile(const scanner::CodeFragment& fragment) {
    CompileResult result;
    
    // 只处理CHTL片段
    if (fragment.type != scanner::FragmentType::CHTL) {
        result.success = false;
        result.errors.push_back("CHTLCompiler can only compile CHTL fragments");
        return result;
    }
    
    try {
        // 使用简单解析器进行解析
        parser::CHTLSimpleParser simpleParser;
        auto parseResult = simpleParser.parse(fragment.content);
        
        if (!parseResult.success) {
            result.success = false;
            result.errors.push_back(parseResult.error);
            
            // 报告错误到错误报告器
            if (errorReporter_) {
                error::SourceLocation loc;
                loc.filename = "CHTL";
                loc.line = parseResult.errorLine;
                loc.column = parseResult.errorColumn;
                errorReporter_->error(parseResult.error, loc);
            }
            
            return result;
        }
        
        // 生成HTML
        result.output = parser::CHTLSimpleParser::generateHTML(parseResult.root);
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("CHTL compilation error: ") + e.what());
        
        if (errorReporter_) {
            errorReporter_->error(e.what());
        }
    }
    
    return result;
}

void CHTLCompilerWrapper::setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter) {
    errorReporter_ = reporter;
}

} // namespace handwritten
} // namespace compiler
} // namespace chtl