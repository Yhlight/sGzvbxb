#include "JSCompilerWrapper.h"
#include "../../error/ErrorCollector.h"

namespace chtl {
namespace compiler {
namespace antlr {

JSCompilerWrapper::JSCompilerWrapper(std::shared_ptr<CHTLContext> context)
    : jsCompiler_(std::make_unique<JSCompiler>(context)),
      errorReporter_(error::ErrorManager::getInstance().getReporter()) {
    jsCompiler_->setErrorReporter(errorReporter_);
}

CompileResult JSCompilerWrapper::compile(const scanner::CodeFragment& fragment) {
    CompileResult result;
    
    // 只处理原生JS片段
    if (fragment.type != scanner::FragmentType::JS) {
        result.success = false;
        result.errors.push_back("JSCompiler can only compile native JavaScript fragments");
        return result;
    }
    
    try {
        // 使用JS编译器编译
        std::string compiledJS = jsCompiler_->compile(fragment.content);
        
        // 检查是否有错误
        if (compiledJS.empty() && !fragment.content.empty()) {
            result.success = false;
            result.errors.push_back("JavaScript compilation failed");
            return result;
        }
        
        result.output = compiledJS;
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("JavaScript compilation error: ") + e.what());
        
        if (errorReporter_) {
            errorReporter_->error(e.what());
        }
    }
    
    return result;
}

void JSCompilerWrapper::setErrorReporter(std::shared_ptr<error::IErrorReporter> reporter) {
    errorReporter_ = reporter;
    if (jsCompiler_) {
        jsCompiler_->setErrorReporter(reporter);
    }
}

} // namespace antlr
} // namespace compiler
} // namespace chtl