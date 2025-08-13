#include "CSSCompilerWrapper.h"
#include "../../error/ErrorCollector.h"

namespace chtl {
namespace compiler {
namespace antlr {

CSSCompilerWrapper::CSSCompilerWrapper(std::shared_ptr<CHTLContext> context)
    : cssCompiler_(std::make_unique<CSSCompiler>(context)),
      errorReporter_(error::ErrorManager::getInstance().getReporter()) {
    cssCompiler_->setErrorReporter(errorReporter_);
}

CompileResult CSSCompilerWrapper::compile(const scanner::CodeFragment& fragment) {
    CompileResult result;
    
    // 只处理CSS片段
    if (fragment.type != scanner::FragmentType::CSS) {
        result.success = false;
        result.errors.push_back("CSSCompiler can only compile CSS fragments");
        return result;
    }
    
    try {
        // 收集错误
        std::vector<std::string> errors;
        
        // 使用CSS编译器编译
        std::string compiledCSS = cssCompiler_->compile(fragment.content);
        
        // 检查是否有错误
        if (compiledCSS.empty() && !fragment.content.empty()) {
            result.success = false;
            result.errors.push_back("CSS compilation failed");
            return result;
        }
        
        result.output = compiledCSS;
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("CSS compilation error: ") + e.what());
        
        if (errorReporter_) {
            errorReporter_->error(e.what());
        }
    }
    
    return result;
}

void CSSCompilerWrapper::setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter) {
    errorReporter_ = reporter;
    if (cssCompiler_) {
        cssCompiler_->setErrorReporter(reporter);
    }
}

} // namespace antlr
} // namespace compiler
} // namespace chtl