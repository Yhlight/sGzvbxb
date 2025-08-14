#include "CHTLJSErrorReporter.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace chtl {

void CHTLJSErrorReporter::reportError(CHTLJSErrorType type, const std::string& message) {
    if (errors.size() >= static_cast<size_t>(maxErrorCount)) {
        return; // 达到最大错误数限制
    }
    
    errors.emplace_back(type, message);
    
    // 立即输出错误
    if (context) {
        context->reportError(formatError(errors.back()));
    }
}

void CHTLJSErrorReporter::reportError(CHTLJSErrorType type, const std::string& message, 
                                     const ErrorLocation& location) {
    if (errors.size() >= static_cast<size_t>(maxErrorCount)) {
        return;
    }
    
    errors.emplace_back(type, message, location);
    
    if (context) {
        context->reportError(formatError(errors.back()));
    }
}

void CHTLJSErrorReporter::reportError(const CHTLJSError& error) {
    if (errors.size() >= static_cast<size_t>(maxErrorCount)) {
        return;
    }
    
    errors.push_back(error);
    
    if (context) {
        context->reportError(formatError(error));
    }
}

void CHTLJSErrorReporter::reportWarning(const std::string& message) {
    CHTLJSError warning(CHTLJSErrorType::Warning, message);
    
    if (treatWarningsAsErrors) {
        reportError(warning);
    } else {
        warnings.push_back(warning);
        
        if (context && true) {
            std::cerr << formatError(warning) << std::endl;
        }
    }
}

void CHTLJSErrorReporter::reportWarning(const std::string& message, const ErrorLocation& location) {
    CHTLJSError warning(CHTLJSErrorType::Warning, message, location);
    
    if (treatWarningsAsErrors) {
        reportError(warning);
    } else {
        warnings.push_back(warning);
        
        if (context && true) {
            std::cerr << formatError(warning) << std::endl;
        }
    }
}

void CHTLJSErrorReporter::addSuggestion(const std::string& suggestion) {
    if (!errors.empty()) {
        errors.back().suggestion = suggestion;
    }
}

std::string CHTLJSErrorReporter::formatError(const CHTLJSError& error) const {
    std::stringstream ss;
    
    // 错误类型和级别
    std::string prefix = (error.type == CHTLJSErrorType::Warning) ? "warning" : "error";
    std::string typeStr = getErrorTypeString(error.type);
    
    // 位置信息
    if (!error.location.file.empty() && error.location.line > 0) {
        ss << error.location.file << ":" << error.location.line;
        if (error.location.column > 0) {
            ss << ":" << error.location.column;
        }
        ss << ": ";
    }
    
    // 错误级别和类型
    ss << prefix << ": [" << typeStr << "] ";
    
    // 错误消息
    ss << error.message;
    
    // 上下文行
    if (!error.location.contextLine.empty()) {
        ss << "\n" << error.location.contextLine;
        
        // 错误位置指示器
        if (error.location.column > 0) {
            ss << "\n" << std::string(error.location.column - 1, ' ') << "^";
        }
    }
    
    // 修复建议
    if (!error.suggestion.empty()) {
        ss << "\n" << "suggestion: " << error.suggestion;
    }
    
    return ss.str();
}

std::string CHTLJSErrorReporter::formatLocation(const ErrorLocation& location) const {
    std::stringstream ss;
    
    if (!location.file.empty()) {
        ss << location.file;
        if (location.line > 0) {
            ss << ":" << location.line;
            if (location.column > 0) {
                ss << ":" << location.column;
            }
        }
    }
    
    return ss.str();
}

void CHTLJSErrorReporter::printAllErrors() const {
    // 打印所有错误
    for (const auto& error : errors) {
        std::cerr << formatError(error) << std::endl << std::endl;
    }
    
    // 打印所有警告
    for (const auto& warning : warnings) {
        std::cerr << formatError(warning) << std::endl << std::endl;
    }
}

void CHTLJSErrorReporter::printSummary() const {
    if (errors.empty() && warnings.empty()) {
        return;
    }
    
    std::cerr << "\n=== CHTL JS Compilation Summary ===" << std::endl;
    
    if (!errors.empty()) {
        std::cerr << "Errors: " << errors.size();
        if (errors.size() >= static_cast<size_t>(maxErrorCount)) {
            std::cerr << " (maximum reached)";
        }
        std::cerr << std::endl;
    }
    
    if (!warnings.empty()) {
        std::cerr << "Warnings: " << warnings.size() << std::endl;
    }
    
    std::cerr << "===================================" << std::endl;
}

std::string CHTLJSErrorReporter::getErrorTypeString(CHTLJSErrorType type) {
    switch (type) {
        case CHTLJSErrorType::SyntaxError: return "SyntaxError";
        case CHTLJSErrorType::ReferenceError: return "ReferenceError";
        case CHTLJSErrorType::UndefinedVariable: return "UndefinedVariable";
        case CHTLJSErrorType::UndefinedFunction: return "UndefinedFunction";
        case CHTLJSErrorType::TypeError: return "TypeError";
        case CHTLJSErrorType::InvalidSelector: return "InvalidSelector";
        case CHTLJSErrorType::InvalidMethodCall: return "InvalidMethodCall";
        case CHTLJSErrorType::RuntimeError: return "RuntimeError";
        case CHTLJSErrorType::DivisionByZero: return "DivisionByZero";
        case CHTLJSErrorType::NullPointerAccess: return "NullPointerAccess";
        case CHTLJSErrorType::SemanticError: return "SemanticError";
        case CHTLJSErrorType::DuplicateDeclaration: return "DuplicateDeclaration";
        case CHTLJSErrorType::ConstReassignment: return "ConstReassignment";
        case CHTLJSErrorType::TemporalDeadZone: return "TemporalDeadZone";
        case CHTLJSErrorType::InvalidCHTLSyntax: return "InvalidCHTLSyntax";
        case CHTLJSErrorType::InvalidEnhancedSelector: return "InvalidEnhancedSelector";
        case CHTLJSErrorType::InvalidArrowSyntax: return "InvalidArrowSyntax";
        case CHTLJSErrorType::InvalidListenConfig: return "InvalidListenConfig";
        case CHTLJSErrorType::InvalidDelegateConfig: return "InvalidDelegateConfig";
        case CHTLJSErrorType::InvalidAnimateConfig: return "InvalidAnimateConfig";
        case CHTLJSErrorType::Warning: return "Warning";
        case CHTLJSErrorType::UnusedVariable: return "UnusedVariable";
        case CHTLJSErrorType::UnreachableCode: return "UnreachableCode";
        case CHTLJSErrorType::DeprecatedSyntax: return "DeprecatedSyntax";
        default: return "Unknown";
    }
}

// 常见错误的工厂方法
CHTLJSError CHTLJSErrorReporter::undefinedVariable(const std::string& varName, const ErrorLocation& loc) {
    CHTLJSError error(CHTLJSErrorType::UndefinedVariable, 
                      "undefined variable '" + varName + "'", loc);
    error.suggestion = "Did you mean to declare it with 'let', 'const', or 'var'?";
    return error;
}

CHTLJSError CHTLJSErrorReporter::invalidSelector(const std::string& selector, const ErrorLocation& loc) {
    CHTLJSError error(CHTLJSErrorType::InvalidSelector,
                      "invalid CHTL selector '" + selector + "'", loc);
    error.suggestion = "CHTL selectors should be in the format {{selector}} or {{#id}} or {{.class}}";
    return error;
}

CHTLJSError CHTLJSErrorReporter::duplicateDeclaration(const std::string& name, const ErrorLocation& loc) {
    CHTLJSError error(CHTLJSErrorType::DuplicateDeclaration,
                      "duplicate declaration of '" + name + "'", loc);
    error.suggestion = "Variable '" + name + "' has already been declared in this scope";
    return error;
}

CHTLJSError CHTLJSErrorReporter::constReassignment(const std::string& varName, const ErrorLocation& loc) {
    CHTLJSError error(CHTLJSErrorType::ConstReassignment,
                      "cannot reassign const variable '" + varName + "'", loc);
    error.suggestion = "Use 'let' instead of 'const' if you need to reassign this variable";
    return error;
}

} // namespace chtl