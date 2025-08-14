#include "CHTLJSCodeGenerator.h"
#include <regex>

namespace chtl {
namespace parser {

CHTLJSCodeGenerator::CHTLJSCodeGenerator() : indentLevel_(0) {}

std::string CHTLJSCodeGenerator::generate(std::shared_ptr<ParseContext> parseTree) {
    output_.str("");
    output_.clear();
    indentLevel_ = 0;
    selectorCache_.clear();
    
    // 生成CHTL JS运行时函数
    writeLine("/* CHTL JS Runtime Functions */");
    writeLine("const _chtl = {");
    indent();
    
    // 选择器函数
    writeLine("select: function(selector) {");
    indent();
    writeLine("if (selector.startsWith('.')) {");
    indent();
    writeLine("return document.querySelectorAll(selector);");
    dedent();
    writeLine("} else if (selector.startsWith('#')) {");
    indent();
    writeLine("return document.querySelector(selector);");
    dedent();
    writeLine("} else {");
    indent();
    writeLine("// 先尝试ID，再尝试类，最后尝试标签");
    writeLine("let elem = document.getElementById(selector);");
    writeLine("if (elem) return elem;");
    writeLine("elem = document.querySelector('.' + selector);");
    writeLine("if (elem) return elem;");
    writeLine("return document.querySelectorAll(selector);");
    dedent();
    writeLine("}");
    dedent();
    writeLine("},");
    
    // listen函数
    writeLine("listen: function(elem, events) {");
    indent();
    writeLine("if (!elem) return;");
    writeLine("const elems = elem.length ? Array.from(elem) : [elem];");
    writeLine("Object.entries(events).forEach(([event, handler]) => {");
    indent();
    writeLine("elems.forEach(el => el.addEventListener(event, handler));");
    dedent();
    writeLine("});");
    dedent();
    writeLine("},");
    
    // animate函数
    writeLine("animate: function(options) {");
    indent();
    writeLine("const duration = options.duration || 1000;");
    writeLine("const easing = options.easing || 'ease-in-out';");
    writeLine("const elem = options.element;");
    writeLine("if (!elem) return;");
    writeLine("");
    writeLine("const keyframes = [];");
    writeLine("if (options.begin) keyframes.push(options.begin);");
    writeLine("if (options.when) {");
    indent();
    writeLine("options.when.forEach(step => {");
    indent();
    writeLine("const frame = { ...step };");
    writeLine("delete frame.at;");
    writeLine("frame.offset = step.at;");
    writeLine("keyframes.push(frame);");
    dedent();
    writeLine("});");
    dedent();
    writeLine("}");
    writeLine("if (options.end) keyframes.push(options.end);");
    writeLine("");
    writeLine("elem.animate(keyframes, { duration, easing });");
    dedent();
    writeLine("}");
    
    dedent();
    writeLine("};");
    writeLine("");
    writeLine("/* Generated CHTL JS Code */");
    
    // 生成主代码
    for (auto child : parseTree->getChildren()) {
        auto ctx = std::dynamic_pointer_cast<ParseContext>(child);
        if (ctx) {
            generateNode(ctx);
        }
    }
    
    return output_.str();
}

void CHTLJSCodeGenerator::generateNode(std::shared_ptr<ParseContext> node) {
    if (!node) return;
    
    if (node->getName() == "chtlInterpolation") {
        generateInterpolation(node);
    } else if (node->getName() == "chainAccess") {
        generateChainAccess(node);
    } else if (node->getName() == "listenCall") {
        generateListen(node);
    } else if (node->getName() == "animateCall") {
        generateAnimate(node);
    } else if (node->getName() == "rawJavaScript") {
        generateRawJS(node);
    } else {
        // 递归处理子节点
        for (auto child : node->getChildren()) {
            auto ctx = std::dynamic_pointer_cast<ParseContext>(child);
            if (ctx) {
                generateNode(ctx);
            }
        }
    }
}

void CHTLJSCodeGenerator::generateInterpolation(std::shared_ptr<ParseContext> node) {
    // {{selector}} -> _chtl.select('selector')
    std::string selector;
    for (auto child : node->getChildren()) {
        if (auto terminal = std::dynamic_pointer_cast<TerminalNode>(child)) {
            std::string text = terminal->getToken()->getText();
            if (text != "{{" && text != "}}") {
                selector += text;
            }
        }
    }
    
    // 处理数组访问 如 button[0]
    std::regex arrayAccess(R"((\w+)\[(\d+)\])");
    std::smatch match;
    if (std::regex_match(selector, match, arrayAccess)) {
        write("_chtl.select('" + match[1].str() + "')[" + match[2].str() + "]");
    } else {
        write("_chtl.select('" + selector + "')");
    }
}

void CHTLJSCodeGenerator::generateChainAccess(std::shared_ptr<ParseContext> node) {
    // 将 -> 转换为 .
    for (auto child : node->getChildren()) {
        if (auto terminal = std::dynamic_pointer_cast<TerminalNode>(child)) {
            if (terminal->getToken()->getText() == "->") {
                write(".");
            } else {
                write(terminal->getToken()->getText());
            }
        } else {
            auto ctx = std::dynamic_pointer_cast<ParseContext>(child);
            if (ctx) {
                generateNode(ctx);
            }
        }
    }
}

void CHTLJSCodeGenerator::generateListen(std::shared_ptr<ParseContext> node) {
    // elem.listen({ click: handler, ... })
    write("_chtl.listen(");
    
    bool first = true;
    for (auto child : node->getChildren()) {
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (!first && childCtx && childCtx->getName() != "listenEvents") {
            write(", ");
        }
        if (childCtx && childCtx->getName() == "listenTarget") {
            generateNode(childCtx);
            first = false;
        } else if (childCtx && childCtx->getName() == "listenEvents") {
            // 生成事件对象
            write("{");
            indent();
            
            bool firstEvent = true;
            for (auto eventChild : childCtx->getChildren()) {
                auto eventCtx = std::dynamic_pointer_cast<ParseContext>(eventChild);
                if (eventCtx && eventCtx->getName() == "listenEvent") {
                    if (!firstEvent) write(",");
                    writeLine("");
                    writeIndent();
                    
                    // 获取事件名和处理器
                    std::string eventName;
                    for (auto part : eventCtx->getChildren()) {
                        auto partCtx = std::dynamic_pointer_cast<ParseContext>(part);
                        if (partCtx && partCtx->getName() == "eventName") {
                            if (!partCtx->getChildren().empty()) {
                                if (auto terminal = std::dynamic_pointer_cast<TerminalNode>(partCtx->getChildren()[0])) {
                                    eventName = terminal->getToken()->getText();
                                write(eventName + ": ");
                                }
                            }
                        } else if (partCtx && partCtx->getName() == "eventHandler") {
                            generateNode(partCtx);
                        }
                    }
                    firstEvent = false;
                }
            }
            
            dedent();
            writeLine("");
            writeIndent();
            write("}");
        }
    }
    
    write(")");
}

void CHTLJSCodeGenerator::generateAnimate(std::shared_ptr<ParseContext> node) {
    // animate({ duration: 1000, ... })
    write("_chtl.animate(");
    
    // 直接生成animate的参数对象
    for (auto child : node->getChildren()) {
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx && childCtx->getName() == "animateOptions") {
            generateNode(childCtx);
        }
    }
    
    write(")");
}

void CHTLJSCodeGenerator::generateRawJS(std::shared_ptr<ParseContext> node) {
    // 原始JavaScript代码
    for (auto child : node->getChildren()) {
        if (auto terminal = std::dynamic_pointer_cast<TerminalNode>(child)) {
            write(terminal->getToken()->getText());
        }
    }
}

void CHTLJSCodeGenerator::indent() {
    indentLevel_++;
}

void CHTLJSCodeGenerator::dedent() {
    if (indentLevel_ > 0) indentLevel_--;
}

void CHTLJSCodeGenerator::writeIndent() {
    for (int i = 0; i < indentLevel_; i++) {
        output_ << "    ";
    }
}

void CHTLJSCodeGenerator::write(const std::string& text) {
    output_ << text;
}

void CHTLJSCodeGenerator::writeLine(const std::string& text) {
    writeIndent();
    output_ << text << std::endl;
}

} // namespace parser
} // namespace chtl