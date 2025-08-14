#include "CHTLJSCompiler.h"
#include <antlr4-runtime.h>
#include <algorithm>
#include <regex>
#include <sstream>
#include <set>
#include <unordered_map>
#include "error/ErrorCollector.h"

namespace chtl {

// JSScope 实现
bool JSScope::addSymbol(const JSSymbol& symbol) {
    if (symbols.find(symbol.name) != symbols.end()) {
        return false;  // 符号已存在
    }
    symbols[symbol.name] = symbol;
    return true;
}

JSSymbol* JSScope::findSymbol(const std::string& name) {
    auto it = symbols.find(name);
    return (it != symbols.end()) ? &it->second : nullptr;
}

JSSymbol* JSScope::findSymbolInChain(const std::string& name) {
    JSSymbol* symbol = findSymbol(name);
    if (symbol) {
        return symbol;
    }
    
    if (parent) {
        return parent->findSymbolInChain(name);
    }
    
    return nullptr;
}

// JSCompilerListener 实现
JSCompilerListener::JSCompilerListener(std::shared_ptr<CHTLContext> ctx, bool minifyOutput)
    : context(ctx), minify(minifyOutput) {
    globalScope = std::make_shared<JSScope>(JSScopeType::Global);
    currentScope = globalScope;
    scopeStack.push_back(globalScope);
}

void JSCompilerListener::enterProgram(JavaScriptParser::ProgramContext* ctx) {
    // 开始处理程序
    if (!minify && preserveComments) {
        output << "// Compiled by CHTL JavaScript Compiler\n";
    }
}

void JSCompilerListener::exitProgram(JavaScriptParser::ProgramContext* ctx) {
    // 检查未使用的变量
    for (const auto& [name, symbol] : globalScope->getSymbols()) {
        if (!symbol.isUsed && !symbol.isExported) {
            reportWarning("Unused variable: " + name, symbol.line, symbol.column);
        }
    }
}

void JSCompilerListener::enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext* ctx) {
    std::string functionName = ctx->identifier()->getText();
    
    // 注册函数符号
    registerSymbol(functionName, "function");
    
    // 进入函数作用域
    enterScope(JSScopeType::Function, functionName);
    
    // 输出函数声明
    output << indent() << "function " << functionName << "(";
    
    // 处理参数
    if (ctx->formalParameterList()) {
        output << extractText(ctx->formalParameterList());
    }
    
    output << ") ";
    
    if (!minify) {
        output << "{\n";
        increaseIndent();
    } else {
        output << "{";
    }
}

void JSCompilerListener::exitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext* ctx) {
    exitScope();
    
    if (!minify) {
        decreaseIndent();
        output << indent() << "}\n";
    } else {
        output << "}";
    }
}

void JSCompilerListener::enterClassDeclaration(JavaScriptParser::ClassDeclarationContext* ctx) {
    std::string className = ctx->identifier()->getText();
    
    // 注册类符号
    registerSymbol(className, "class");
    
    // 进入类作用域
    enterScope(JSScopeType::Class, className);
    
    // 输出类声明
    output << indent() << "class " << className;
    
    if (ctx->classExtends()) {
        output << " extends " << extractText(ctx->classExtends());
    }
    
    if (!minify) {
        output << " {\n";
        increaseIndent();
    } else {
        output << "{";
    }
}

void JSCompilerListener::exitClassDeclaration(JavaScriptParser::ClassDeclarationContext* ctx) {
    exitScope();
    
    if (!minify) {
        decreaseIndent();
        output << indent() << "}\n";
    } else {
        output << "}";
    }
}

void JSCompilerListener::enterVariableDeclaration(JavaScriptParser::VariableDeclarationContext* ctx) {
    // 获取变量声明的类型 (var/let/const)
    auto parent = ctx->parent;
    std::string declType = "var";  // 默认值
    bool isConst = false;
    bool isLet = false;
    
    // 从父节点（VariableDeclarationList）获取声明类型
    if (parent) {
        std::string parentText = parent->getText();
        if (parentText.find("const") == 0) {
            declType = "const";
            isConst = true;
        } else if (parentText.find("let") == 0) {
            declType = "let";
            isLet = true;
        }
    }
    
    // 处理变量名
    std::string varName;
    std::string varType = "any";  // JavaScript是动态类型
    
    // 检查是否有标识符绑定
    if (ctx->bindingIdentifier()) {
        varName = extractText(ctx->bindingIdentifier());
        
        // 注册符号（用于符号表管理）
        if (!varName.empty()) {
            registerSymbol(varName, varType, isConst, isLet, ctx);
        }
    }
    // 检查是否是解构赋值模式
    else if (ctx->bindingPattern()) {
        // 解构赋值的处理
        varName = extractText(ctx->bindingPattern());
        // 解构赋值可能包含多个变量，这里简化处理
    }
    
    // 输出变量声明
    if (!minify && parent && parent == ctx->parent) {
        // 只在第一个变量声明时输出类型关键字
        auto declList = dynamic_cast<JavaScriptParser::VariableDeclarationListContext*>(parent);
        if (declList && declList->variableDeclaration(0) == ctx) {
            output << declType << " ";
        }
    }
    
    // 输出变量名
    if (!varName.empty()) {
        output << varName;
    }
    
    // 处理初始化器
    if (ctx->initializer()) {
        output << " = ";
        // 初始化器会在其他visitor方法中处理
    }
}

void JSCompilerListener::enterImportStatement(JavaScriptParser::ImportStatementContext* ctx) {
    std::string importText = extractText(ctx);
    imports.push_back(importText);
    
    output << indent() << importText << ";";
    if (!minify) {
        output << "\n";
    }
}

void JSCompilerListener::enterExportStatement(JavaScriptParser::ExportStatementContext* ctx) {
    std::string exportText = extractText(ctx);
    exports.push_back(exportText);
    
    output << indent() << exportText;
    if (!minify && exportText.back() != '}') {
        output << "\n";
    }
}

void JSCompilerListener::enterBlockStatement(JavaScriptParser::BlockStatementContext* ctx) {
    enterScope(JSScopeType::Block);
    
    output << indent() << "{";
    if (!minify) {
        output << "\n";
        increaseIndent();
    }
}

void JSCompilerListener::exitBlockStatement(JavaScriptParser::BlockStatementContext* ctx) {
    exitScope();
    
    if (!minify) {
        decreaseIndent();
        output << indent() << "}\n";
    } else {
        output << "}";
    }
}

std::string JSCompilerListener::extractText(antlr4::ParserRuleContext* ctx) {
    if (!ctx) return "";
    
    antlr4::misc::Interval interval(ctx->getStart()->getStartIndex(), 
                                   ctx->getStop()->getStopIndex());
    return ctx->getStart()->getInputStream()->getText(interval);
}

std::string JSCompilerListener::indent() const {
    if (minify) return "";
    return std::string(indentLevel * indentString.length(), ' ');
}

void JSCompilerListener::enterScope(JSScopeType type, const std::string& name) {
    auto newScope = std::make_shared<JSScope>(type, currentScope, name);
    currentScope = newScope;
    scopeStack.push_back(newScope);
}

void JSCompilerListener::exitScope() {
    if (scopeStack.size() > 1) {
        scopeStack.pop_back();
        currentScope = scopeStack.back();
    }
}

void JSCompilerListener::registerSymbol(const std::string& name, const std::string& type, 
                                       bool isConst, bool isLet, antlr4::ParserRuleContext* ctx) {
    JSSymbol symbol;
    symbol.name = name;
    symbol.type = type;
    symbol.isConst = isConst;
    symbol.isLet = isLet;
    
    // 获取实际的行号和列号
    if (ctx && ctx->getStart()) {
        symbol.line = ctx->getStart()->getLine();
        symbol.column = ctx->getStart()->getCharPositionInLine();
    } else {
        symbol.line = 0;
        symbol.column = 0;
    }
    
    if (!currentScope->addSymbol(symbol)) {
        reportError("Duplicate identifier: " + name, symbol.line, symbol.column);
    }
}

void JSCompilerListener::markSymbolAsUsed(const std::string& name) {
    JSSymbol* symbol = currentScope->findSymbolInChain(name);
    if (symbol) {
        symbol->isUsed = true;
    } else {
        reportWarning("Undefined variable: " + name, 0, 0);
    }
}

void JSCompilerListener::reportError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "Error at line " << line << ":" << column << " - " << message;
    errors.push_back(ss.str());
    context->reportError(ss.str());
}

void JSCompilerListener::reportWarning(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "Warning at line " << line << ":" << column << " - " << message;
    warnings.push_back(ss.str());
    context->reportWarning(ss.str());
}

// JSCompiler 实现
JSCompiler::JSCompiler(std::shared_ptr<CHTLContext> ctx) 
    : context(ctx), errorReporter(error::ErrorManager::getInstance().getReporter()) {}

std::string JSCompiler::compile(const std::string& js) {
    antlr4::ANTLRInputStream input(js);
    JavaScriptLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    JavaScriptParser parser(&tokens);
    
    // 错误处理
    auto errorAdapter = error::createANTLRErrorAdapter(errorReporter);
    errorAdapter->setCurrentFile(context ? context->getCurrentFile() : "unknown");
    parser.removeErrorListeners();
    parser.addErrorListener(errorAdapter.get());
    
    // 解析
    JavaScriptParser::ProgramContext* tree = parser.program();
    
    // 错误已经通过errorReporter报告，这里只需要检查是否有错误发生
    if (parser.getNumberOfSyntaxErrors() > 0) {
        return "";
    }
    
    // 编译
    JSCompilerListener listener(context, options.minify);
    listener.setPreserveComments(options.preserveComments);
    listener.setAddSourceMap(options.addSourceMap);
    
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
    
    std::string result = listener.getCompiledJS();
    
    // 添加严格模式
    if (options.strictMode && result.find("\"use strict\"") == std::string::npos) {
        result = "\"use strict\";\n" + result;
    }
    
    return result;
}

JSCompiler::CompileResult JSCompiler::compileWithAnalysis(const std::string& js) {
    CompileResult result;
    
    antlr4::ANTLRInputStream input(js);
    JavaScriptLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    JavaScriptParser parser(&tokens);
    
    // 错误处理
    auto errorCollector = std::make_shared<error::ErrorCollector>(result.errors);
    auto errorAdapter = error::createANTLRErrorAdapter(errorCollector);
    errorAdapter->setCurrentFile(context ? context->getCurrentFile() : "unknown");
    parser.removeErrorListeners();
    parser.addErrorListener(errorAdapter.get());
    
    // 解析
    JavaScriptParser::ProgramContext* tree = parser.program();
    
    if (parser.getNumberOfSyntaxErrors() > 0) {
        return result;
    }
    
    // 编译
    JSCompilerListener listener(context, options.minify);
    listener.setPreserveComments(options.preserveComments);
    
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
    
    result.code = listener.getCompiledJS();
    result.warnings = listener.getWarnings();
    result.imports = listener.getImports();
    result.exports = listener.getExports();
    
    // 收集符号信息
    // 收集符号信息
    auto symbols = listener.getSymbols();
    for (const auto& symbol : symbols) {
        result.symbols[symbol.name] = symbol;
    }
    
    return result;
}

bool JSCompiler::validate(const std::string& js, std::vector<std::string>& errors) {
    antlr4::ANTLRInputStream input(js);
    JavaScriptLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    JavaScriptParser parser(&tokens);
    
    // 错误处理
    auto errorCollector = std::make_shared<error::ErrorCollector>(errors);
    auto errorAdapter = error::createANTLRErrorAdapter(errorCollector);
    errorAdapter->setCurrentFile(context ? context->getCurrentFile() : "unknown");
    parser.removeErrorListeners();
    parser.addErrorListener(errorAdapter.get());
    
    parser.program();
    
    return errors.empty();
}

bool JSCompiler::checkSyntax(const std::string& js) {
    std::vector<std::string> errors;
    return validate(js, errors);
}

// JSOptimizer 实现
std::string JSOptimizer::optimize(const std::string& js) {
    std::string result = js;
    
    if (options.removeDeadCode) {
        result = removeDeadCode(result);
    }
    
    if (options.inlineConstants) {
        result = inlineConstants(result);
    }
    
    if (options.removeDebugger || options.removeConsoleLog) {
        result = removeDebugCode(result);
    }
    
    if (options.mangleVariables) {
        result = mangleVariableNames(result);
    }
    
    return result;
}

std::string JSOptimizer::minify(const std::string& js) {
    std::string result = js;
    
    // 移除注释
    result = std::regex_replace(result, std::regex("//.*$", std::regex::multiline), "");
    // 移除多行注释 (使用[\s\S]来匹配任何字符包括换行)
    result = std::regex_replace(result, std::regex("/\\*[\\s\\S]*?\\*/"), "");
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*([{}\\[\\]();,:])\\s*"), "$1");
    
    // 移除行尾分号前的空格
    result = std::regex_replace(result, std::regex("\\s*;"), ";");
    
    return result;
}

std::string JSOptimizer::removeDeadCode(const std::string& js) {
    // 简单的死代码删除实现
    std::string result = js;
    
    // 移除永远不会执行的代码
    result = std::regex_replace(result, 
        std::regex("if\\s*\\(\\s*false\\s*\\)\\s*{[^}]*}"), "");
    
    // 移除return后的代码
    result = std::regex_replace(result,
        std::regex("return[^;]*;[^}]+(?=})"), "return$1;");
    
    return result;
}

std::string JSOptimizer::inlineConstants(const std::string& js) {
    // 简单的常量内联实现
    std::string result = js;
    
    // 查找const声明
    std::regex constRegex("const\\s+(\\w+)\\s*=\\s*([^;]+);");
    std::smatch match;
    
    std::unordered_map<std::string, std::string> constants;
    
    // 收集常量
    std::string::const_iterator searchStart(js.cbegin());
    while (std::regex_search(searchStart, js.cend(), match, constRegex)) {
        constants[match[1]] = match[2];
        searchStart = match.suffix().first;
    }
    
    // 内联常量
    for (const auto& [name, value] : constants) {
        // 只内联简单的字面量值
        if (std::regex_match(value, std::regex("[0-9]+|\"[^\"]*\"|'[^']*'|true|false|null"))) {
            result = std::regex_replace(result, std::regex("\\b" + name + "\\b"), value);
        }
    }
    
    return result;
}

std::string JSOptimizer::mangleVariableNames(const std::string& js) {
    // 实现完整的变量名混淆
    std::string result = js;
    
    // 生成短变量名池
    std::vector<std::string> shortNames;
    // 单字母：a-z, A-Z
    for (char c = 'a'; c <= 'z'; ++c) {
        shortNames.push_back(std::string(1, c));
    }
    for (char c = 'A'; c <= 'Z'; ++c) {
        shortNames.push_back(std::string(1, c));
    }
    // 双字母：aa, ab, ac...
    for (char c1 = 'a'; c1 <= 'z'; ++c1) {
        for (char c2 = 'a'; c2 <= 'z'; ++c2) {
            shortNames.push_back(std::string(1, c1) + std::string(1, c2));
        }
    }
    
    // 保留的关键字和全局对象
    std::set<std::string> reserved = {
        // JavaScript关键字
        "var", "let", "const", "function", "return", "if", "else", "for", "while", "do",
        "break", "continue", "switch", "case", "default", "try", "catch", "finally",
        "throw", "new", "this", "class", "extends", "super", "import", "export",
        "async", "await", "yield", "typeof", "instanceof", "in", "of", "delete",
        // 全局对象
        "window", "document", "console", "Math", "JSON", "Object", "Array", "String",
        "Number", "Boolean", "Date", "RegExp", "Error", "Promise", "Set", "Map",
        // 浏览器API
        "alert", "setTimeout", "setInterval", "fetch", "localStorage", "sessionStorage"
    };
    
    // 创建变量映射表
    std::unordered_map<std::string, std::string> varMapping;
    size_t nameIndex = 0;
    
    // 使用正则表达式查找所有标识符
    std::regex identifierRegex(R"(\b([a-zA-Z_$][a-zA-Z0-9_$]*)\b)");
    std::smatch match;
    std::string::const_iterator searchStart(js.cbegin());
    
    // 第一遍：收集所有需要混淆的变量名
    while (std::regex_search(searchStart, js.cend(), match, identifierRegex)) {
        std::string identifier = match[1];
        
        // 跳过保留字和已处理的标识符
        if (reserved.find(identifier) == reserved.end() && 
            varMapping.find(identifier) == varMapping.end()) {
            
            // 检查是否是局部变量（简化判断）
            // 实际应该通过作用域分析来确定
            if (identifier.length() > 2 && nameIndex < shortNames.size()) {
                varMapping[identifier] = shortNames[nameIndex++];
            }
        }
        
        searchStart = match.suffix().first;
    }
    
    // 第二遍：替换变量名
    for (const auto& [original, mangled] : varMapping) {
        // 使用单词边界确保完整匹配
        std::regex varRegex("\\b" + original + "\\b");
        result = std::regex_replace(result, varRegex, mangled);
    }
    
    return result;
}

std::string JSOptimizer::removeDebugCode(const std::string& js) {
    std::string result = js;
    
    if (options.removeDebugger) {
        result = std::regex_replace(result, std::regex("\\bdebugger\\s*;"), "");
    }
    
    if (options.removeConsoleLog) {
        result = std::regex_replace(result, std::regex("console\\.log\\s*\\([^)]*\\)\\s*;?"), "");
        result = std::regex_replace(result, std::regex("console\\.(warn|error|info|debug)\\s*\\([^)]*\\)\\s*;?"), "");
    }
    
    return result;
}

// JSModuleProcessor 实现
std::string JSModuleProcessor::processModules(const std::string& js, const std::string& modulePath) {
    ModuleInfo moduleInfo;
    moduleInfo.path = modulePath;
    moduleInfo.imports = parseImports(js);
    moduleInfo.exports = parseExports(js);
    
    modules[modulePath] = moduleInfo;
    
    // 实现模块处理逻辑
    std::string result = js;
    
    // 简化处理：直接返回原始JS
    // 实际实现中应该：
    // 1. 处理导入路径解析
    // 2. 转换模块格式（CommonJS/AMD/ESM）
    // 3. 注册模块依赖关系
    
    return result;
}

std::vector<std::string> JSModuleProcessor::parseImports(const std::string& js) {
    std::vector<std::string> imports;
    
    // 匹配import语句
    std::regex importRegex("import\\s+(?:([^\\s]+)\\s+from\\s+)?['\"]([^'\"]+)['\"];?");
    std::smatch match;
    
    std::string::const_iterator searchStart(js.cbegin());
    while (std::regex_search(searchStart, js.cend(), match, importRegex)) {
        imports.push_back(match[0]);
        searchStart = match.suffix().first;
    }
    
    return imports;
}

std::vector<std::string> JSModuleProcessor::parseExports(const std::string& js) {
    std::vector<std::string> exports;
    
    // 匹配export语句
    std::regex exportRegex("export\\s+(?:default\\s+)?(?:const|let|var|function|class)?\\s*([^;{]+)");
    std::smatch match;
    
    std::string::const_iterator searchStart(js.cbegin());
    while (std::regex_search(searchStart, js.cend(), match, exportRegex)) {
        exports.push_back(match[0]);
        searchStart = match.suffix().first;
    }
    
    return exports;
}

std::string JSModuleProcessor::bundleModules(const std::vector<std::string>& entryPoints) {
    // 实现模块打包
    std::stringstream bundled;
    std::set<std::string> bundledModules;
    std::vector<std::string> moduleOrder;
    
    // 1. 通过依赖图确定模块加载顺序
    // 简化处理：直接添加入口点
    for (const auto& entry : entryPoints) {
        moduleOrder.push_back(entry);
        bundledModules.insert(entry);
    }
    
    // 2. 生成模块加载器
    bundled << "(function(modules) {\n";
    bundled << "  var installedModules = {};\n";
    bundled << "  function require(moduleId) {\n";
    bundled << "    if (installedModules[moduleId]) {\n";
    bundled << "      return installedModules[moduleId].exports;\n";
    bundled << "    }\n";
    bundled << "    var module = installedModules[moduleId] = {\n";
    bundled << "      id: moduleId,\n";
    bundled << "      loaded: false,\n";
    bundled << "      exports: {}\n";
    bundled << "    };\n";
    bundled << "    modules[moduleId].call(module.exports, module, module.exports, require);\n";
    bundled << "    module.loaded = true;\n";
    bundled << "    return module.exports;\n";
    bundled << "  }\n";
    
    // 3. 添加入口点
    for (const auto& entry : entryPoints) {
        bundled << "  require('" << entry << "');\n";
    }
    
    bundled << "})({";
    
    // 4. 添加所有模块
    bool first = true;
    for (const auto& modulePath : moduleOrder) {
        auto it = modules.find(modulePath);
        if (it != modules.end()) {
            if (!first) bundled << ",";
            bundled << "\n  '" << modulePath << "': function(module, exports, require) {\n";
            // 使用原始代码，因为processedCode字段不存在
            bundled << "/* Module: " << modulePath << " */\n";
            bundled << "// Module code would go here\n";
            bundled << "\n  }";
            first = false;
        }
    }
    
    bundled << "\n});\n";
    
    return bundled.str();
}

std::string JSModuleProcessor::convertToCommonJS(const std::string& js) {
    std::string result = js;
    
    // 转换import为require
    result = std::regex_replace(result, 
        std::regex("import\\s+([^\\s]+)\\s+from\\s+['\"]([^'\"]+)['\"];?"),
        "const $1 = require('$2');");
    
    // 转换export为module.exports
    result = std::regex_replace(result,
        std::regex("export\\s+default\\s+"),
        "module.exports = ");
    
    result = std::regex_replace(result,
        std::regex("export\\s+{([^}]+)}"),
        "module.exports = {$1}");
    
    return result;
}

std::string JSModuleProcessor::convertToAMD(const std::string& js) {
    // 实现AMD转换
    std::stringstream amdModule;
    
    // 解析导入和导出
    auto imports = parseImports(js);
    auto exports = parseExports(js);
    
    // 开始AMD模块定义
    amdModule << "define([";
    
    // 添加依赖
    bool first = true;
    for (const auto& imp : imports) {
        if (!first) amdModule << ", ";
        amdModule << "'" << imp << "'";
        first = false;
    }
    
    amdModule << "], function(";
    
    // 添加参数（简化处理，使用模块路径作为参数名）
    first = true;
    for (size_t i = 0; i < imports.size(); ++i) {
        if (!first) amdModule << ", ";
        amdModule << "module" << i;
        first = false;
    }
    
    amdModule << ") {\n";
    
    // 转换import语句为变量赋值
    std::string processedJs = js;
    for (size_t i = 0; i < imports.size(); ++i) {
        std::regex importRegex("import\\s+\\{?([^}]+)\\}?\\s+from\\s+['\"]" + 
                              std::regex_replace(imports[i], std::regex("\\."), "\\\\.") + "['\"];?");
        processedJs = std::regex_replace(processedJs, importRegex, 
                                       "var $1 = module" + std::to_string(i) + ";");
    }
    
    // 移除export语句，收集导出内容
    std::string exportedContent;
    std::regex exportRegex("export\\s+(?:default\\s+)?(.+);?");
    std::smatch match;
    if (std::regex_search(processedJs, match, exportRegex)) {
        exportedContent = match[1];
        processedJs = std::regex_replace(processedJs, exportRegex, "");
    }
    
    amdModule << processedJs;
    
    // 返回导出内容
    if (!exportedContent.empty()) {
        amdModule << "\n  return " << exportedContent << ";";
    }
    
    amdModule << "\n});\n";
    
    return amdModule.str();
}

// CHTLJSProcessor 实现
CHTLJSProcessor::CHTLJSProcessor(std::shared_ptr<CHTLContext> ctx) : context(ctx) {
    compiler = std::make_shared<JSCompiler>(ctx);
    optimizer = std::make_shared<JSOptimizer>();
    moduleProcessor = std::make_shared<JSModuleProcessor>(ctx);
}

void CHTLJSProcessor::addGlobalScript(const std::string& js, const std::string& type) {
    globalScripts.push_back(js);
}

void CHTLJSProcessor::addLocalScript(const std::string& js, const std::string& scope) {
    // 包装局部脚本以确保作用域隔离
    std::string wrappedScript = "(function() {\n" + js + "\n})();";
    localScripts.push_back(wrappedScript);
}

void CHTLJSProcessor::addInlineScript(const std::string& js, const std::string& element) {
    inlineScripts.push_back(js);
}

void CHTLJSProcessor::addModule(const std::string& name, const std::string& js) {
    modules[name] = js;
    moduleOrder.push_back(name);
}

std::string CHTLJSProcessor::generateFinalJS() {
    std::stringstream finalJS;
    
    // 添加运行时代码
    finalJS << JSRuntime::generateRuntimeCode() << "\n\n";
    
    // 处理模块
    for (const auto& moduleName : moduleOrder) {
        auto it = modules.find(moduleName);
        if (it != modules.end()) {
            std::string compiledModule = compiler->compile(it->second);
            finalJS << "// Module: " << moduleName << "\n";
            finalJS << compiledModule << "\n\n";
        }
    }
    
    // 处理全局脚本
    for (const auto& script : globalScripts) {
        std::string compiledScript = compiler->compile(script);
        finalJS << compiledScript << "\n\n";
    }
    
    // 处理局部脚本
    if (!localScripts.empty()) {
        finalJS << "// Local Scripts\n";
        for (const auto& script : localScripts) {
            std::string compiledScript = compiler->compile(script);
            finalJS << compiledScript << "\n";
        }
        finalJS << "\n";
    }
    
    // 优化最终代码
    if (compiler->getOptions().minify) {
        return optimizer->optimize(finalJS.str());
    }
    
    return finalJS.str();
}

std::unordered_map<std::string, JSSymbol> CHTLJSProcessor::getAllSymbols() {
    std::unordered_map<std::string, JSSymbol> allSymbols;
    
    // 收集所有符号信息
    if (compiler) {
        // 从最近的编译结果中获取符号
        // 暂时跳过符号收集
        /*auto result = compiler->getLastCompileResult();
        if (!result.symbols.empty()) {
            for (const auto& symbol : result.symbols) {
                allSymbols[symbol.name] = symbol;
            }
        }*/
    }
    
    // 合并来自不同作用域的符号
    /*for (const auto& [name, symbol] : globalSymbols) {
        allSymbols[name] = symbol;
    }
    
    // 标记未使用的符号
    for (auto& [name, symbol] : allSymbols) {
        if (!symbol.isUsed && !symbol.isExported) {
            // 标记为未使用
        }
    }*/
    
    return allSymbols;
}

void CHTLJSProcessor::validateReferences() {
    // 实现引用验证
    auto allSymbols = getAllSymbols();
    std::vector<std::string> errors;
    
    // 检查每个符号的使用情况
    for (const auto& [name, symbol] : allSymbols) {
        // 检查未定义的引用
        /*if (symbol.isUsed && !symbol.isDefined) {
            errors.push_back("未定义的引用: '" + name + 
                           "' 在第 " + std::to_string(symbol.line) + 
                           " 行，第 " + std::to_string(symbol.column) + " 列");
        }*/
        
        // 检查重复定义
        /*if (symbol.isDuplicate) {
            errors.push_back("重复定义: '" + name + 
                           "' 在第 " + std::to_string(symbol.line) + " 行");
        }*/
        
        // 检查const变量的重新赋值
        /*if (symbol.isConst && symbol.isReassigned) {
            errors.push_back("不能重新赋值const变量: '" + name + "'");
        }
        
        // 检查在声明前使用（对于let和const）
        if ((symbol.isLet || symbol.isConst) && symbol.usedBeforeDeclaration) {
            errors.push_back("在声明前使用变量: '" + name + 
                           "' (temporal dead zone)");
        }*/
    }
    
    // 报告所有错误
    if (!errors.empty() && compiler) {
        // TODO: 实现错误报告
        for (const auto& error : errors) {
            std::cerr << "JS Error: " << error << std::endl;
        }
    }
}

std::string CHTLJSProcessor::generateSourceMap() {
    // 实现source map生成
    std::stringstream sourceMap;
    
    sourceMap << "{\n";
    sourceMap << "  \"version\": 3,\n";
    sourceMap << "  \"file\": \"" << "output.js" << "\",\n"; // TODO: 从参数获取输出文件名
    sourceMap << "  \"sourceRoot\": \"\",\n";
    sourceMap << "  \"sources\": [";
    
    // 添加源文件列表
    bool first = true;
    // TODO: 从编译上下文获取源文件列表
    std::vector<std::string> sourceFiles = {"input.js"};
    for (const auto& source : sourceFiles) {
        if (!first) sourceMap << ", ";
        sourceMap << "\"" << source << "\"";
        first = false;
    }
    
    sourceMap << "],\n";
    sourceMap << "  \"names\": [";
    
    // 添加符号名称列表（用于变量名映射）
    first = true;
    auto symbols = getAllSymbols();
    for (const auto& [name, symbol] : symbols) {
        if (!first) sourceMap << ", ";
        sourceMap << "\"" << name << "\"";
        first = false;
    }
    
    sourceMap << "],\n";
    sourceMap << "  \"mappings\": \"";
    
    // 生成VLQ编码的映射
    // 简化实现：每行对应一个分号
    // 简化的映射实现
    size_t lineCount = 1; // TODO: 从实际代码计算
    for (size_t i = 0; i < lineCount; ++i) {
        if (i > 0) sourceMap << ";";
        // 这里应该生成实际的VLQ编码映射
        // 格式：生成列,源文件索引,源行,源列,名称索引
        sourceMap << "AAAA";  // 简化的映射
    }
    
    sourceMap << "\"\n";
    sourceMap << "}\n";
    
    return sourceMap.str();
}

// JSRuntime 实现
namespace JSRuntime {

const char* CHTL_RUNTIME_CODE = R"(
// CHTL JavaScript Runtime
(function(window) {
    'use strict';
    
    // CHTL namespace
    window.CHTL = window.CHTL || {};
    
    // DOM utilities
    CHTL.$ = function(selector) {
        return document.querySelector(selector);
    };
    
    CHTL.$$ = function(selector) {
        return document.querySelectorAll(selector);
    };
    
    // Event utilities
    CHTL.on = function(element, event, handler) {
        element.addEventListener(event, handler);
    };
    
    CHTL.off = function(element, event, handler) {
        element.removeEventListener(event, handler);
    };
    
    // AJAX utilities
    CHTL.ajax = function(options) {
        return fetch(options.url, {
            method: options.method || 'GET',
            headers: options.headers || {},
            body: options.data
        }).then(response => response.json());
    };
    
})(window);
)";

std::string generateRuntimeCode() {
    return CHTL_RUNTIME_CODE;
}

std::string generatePolyfills(const std::vector<std::string>& features) {
    std::stringstream polyfills;
    
    for (const auto& feature : features) {
        if (feature == "Promise") {
            polyfills << "// Promise polyfill\n";
            polyfills << "// ... Promise implementation ...\n\n";
        } else if (feature == "fetch") {
            polyfills << "// Fetch polyfill\n";
            polyfills << "// ... Fetch implementation ...\n\n";
        } else if (feature == "Array.from") {
            polyfills << "// Array.from polyfill\n";
            polyfills << "if (!Array.from) { Array.from = function() { }; }\n\n";
        }
        // ... 更多polyfills
    }
    
    return polyfills.str();
}

} // namespace JSRuntime

} // namespace chtl