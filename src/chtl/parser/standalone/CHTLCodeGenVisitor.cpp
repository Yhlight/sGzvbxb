#include "CHTLCodeGenVisitor.h"

namespace chtl {
namespace parser {

std::string CHTLCodeGenVisitor::visit(std::shared_ptr<ParseContext> tree) {
    if (!tree) {
        return "";
    }
    
    // 重置输出流
    html_.str("");
    css_.str("");
    js_.str("");
    
    // 开始访问
    if (tree->getName() == "compilationUnit") {
        visitCompilationUnit(tree);
    }
    
    // 生成最终的 HTML
    std::stringstream output;
    output << "<!DOCTYPE html>\n";
    output << "<html>\n";
    output << "<head>\n";
    output << "    <meta charset=\"UTF-8\">\n";
    output << "    <title>CHTL Generated Page</title>\n";
    
    if (!css_.str().empty()) {
        output << "    <style>\n";
        output << css_.str();
        output << "    </style>\n";
    }
    
    output << "</head>\n";
    output << html_.str();
    
    if (!js_.str().empty()) {
        output << "<script>\n";
        output << js_.str();
        output << "</script>\n";
    }
    
    output << "</html>\n";
    
    return output.str();
}

void CHTLCodeGenVisitor::visitCompilationUnit(std::shared_ptr<ParseContext> ctx) {
    if (!ctx) return;
    
    for (const auto& child : ctx->getChildren()) {
        if (!child) continue;
        
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx) {
            const std::string& name = childCtx->getName();
            
            if (name == "htmlElement") {
                visitHtmlElement(childCtx);
            } else if (name == "templateDefinition") {
                visitTemplateDefinition(childCtx);
            } else if (name == "importStatement") {
                visitImportStatement(childCtx);
            }
        }
    }
}

void CHTLCodeGenVisitor::visitHtmlElement(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().empty()) return;
    
    // 获取元素名
    std::string tagName;
    const auto& children = ctx->getChildren();
    
    if (!children.empty()) {
        auto firstChild = children[0];
        if (firstChild && firstChild->isTerminal()) {
            tagName = firstChild->getText();
        }
    }
    
    if (tagName.empty()) return;
    
    // 生成开始标签
    html_ << "<" << tagName;
    
    // TODO: 处理属性
    
    html_ << ">";
    
    // 处理子元素
    for (size_t i = 1; i < children.size(); ++i) {
        auto child = children[i];
        if (!child) continue;
        
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx) {
            const std::string& name = childCtx->getName();
            
            if (name == "htmlElement") {
                visitHtmlElement(childCtx);
            } else if (name == "textBlock") {
                visitText(childCtx);
            }
        }
    }
    
    // 生成结束标签
    html_ << "</" << tagName << ">";
}

void CHTLCodeGenVisitor::visitText(std::shared_ptr<ParseContext> ctx) {
    if (!ctx) return;
    
    // 提取文本内容
    for (const auto& child : ctx->getChildren()) {
        if (child && child->isTerminal()) {
            std::string text = child->getText();
            // 移除引号
            if (text.length() >= 2 && text.front() == '"' && text.back() == '"') {
                text = text.substr(1, text.length() - 2);
            }
            html_ << text;
        }
    }
}

void CHTLCodeGenVisitor::visitTemplateDefinition(std::shared_ptr<ParseContext> ctx) {
    // TODO: 实现模板定义的处理
}

void CHTLCodeGenVisitor::visitImportStatement(std::shared_ptr<ParseContext> ctx) {
    // TODO: 实现导入语句的处理
}

void CHTLCodeGenVisitor::visitStyleProperty(std::shared_ptr<ParseContext> ctx) {
    // TODO: 实现样式属性的处理
}

std::string CHTLCodeGenVisitor::getNodeText(std::shared_ptr<ParseTree> node) {
    if (!node) return "";
    return node->getText();
}

void CHTLCodeGenVisitor::visitChildren(std::shared_ptr<ParseContext> ctx) {
    if (!ctx) return;
    
    for (const auto& child : ctx->getChildren()) {
        if (!child) continue;
        
        // 递归访问子节点
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx) {
            visit(childCtx);
        }
    }
}

} // namespace parser
} // namespace chtl