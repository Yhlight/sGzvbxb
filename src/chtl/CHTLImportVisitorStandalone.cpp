#include "CHTLImportVisitorStandalone.h"
#include "parser/standalone/CHTLParser.h"
#include "CHTLContext.h"
#include <iostream>

namespace chtl {

// 访问解析树
void CHTLImportVisitorStandalone::visit(std::shared_ptr<parser::ParseContext> tree) {
    if (!tree) return;
    visitChildren(tree);
}

// 访问子节点
void CHTLImportVisitorStandalone::visitChildren(std::shared_ptr<parser::ParseContext> ctx) {
    if (!ctx) return;
    
    // 检查节点类型
    const std::string& nodeName = ctx->getName();
    
    if (nodeName == "templateDefinition") {
        visitTemplateDefinition(ctx);
    } else if (nodeName == "customDefinition") {
        visitCustomDefinition(ctx);
    } else {
        // 递归访问子节点
        for (const auto& child : ctx->getChildren()) {
            if (auto childCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
                visitChildren(childCtx);
            }
        }
    }
}

// 访问模板定义
void CHTLImportVisitorStandalone::visitTemplateDefinition(std::shared_ptr<parser::ParseContext> ctx) {
    // 解析模板定义
    // 格式: [Template] @Type name { ... }
    
    std::string templateType;
    std::string templateName;
    
    // 遍历子节点查找类型和名称
    for (const auto& child : ctx->getChildren()) {
        if (auto terminal = std::dynamic_pointer_cast<parser::TerminalNode>(child)) {
            const auto& token = terminal->getToken();
            if (token->getType() == parser::TokenType::KEYWORD_ELEMENT ||
                token->getType() == parser::TokenType::KEYWORD_STYLE_GROUP ||
                token->getType() == parser::TokenType::KEYWORD_VAR_GROUP) {
                templateType = token->getText().substr(1); // 去掉 @ 前缀
            } else if (token->getType() == parser::TokenType::IDENTIFIER) {
                templateName = token->getText();
            }
        }
    }
    
    // 获取映射后的名称
    std::string mappedName = getMappedName(templateName);
    
    // 根据类型创建相应的模板
    if (templateType == "Style") {
        auto styleTemplate = std::make_shared<StyleTemplate>(mappedName);
        // TODO: 解析样式内容
        templates[mappedName] = styleTemplate;
    } else if (templateType == "Element") {
        auto elementTemplate = std::make_shared<ElementTemplate>(mappedName);
        // TODO: 解析元素内容
        templates[mappedName] = elementTemplate;
    } else if (templateType == "Var") {
        auto varTemplate = std::make_shared<VarTemplate>(mappedName);
        // TODO: 解析变量内容
        templates[mappedName] = varTemplate;
    }
}

// 访问自定义定义
void CHTLImportVisitorStandalone::visitCustomDefinition(std::shared_ptr<parser::ParseContext> ctx) {
    // 解析自定义定义
    // 格式: [Custom] @Type name { ... }
    
    std::string customType;
    std::string customName;
    
    // 遍历子节点查找类型和名称
    for (const auto& child : ctx->getChildren()) {
        if (auto terminal = std::dynamic_pointer_cast<parser::TerminalNode>(child)) {
            const auto& token = terminal->getToken();
            if (token->getType() == parser::TokenType::KEYWORD_ELEMENT ||
                token->getType() == parser::TokenType::KEYWORD_STYLE_GROUP ||
                token->getType() == parser::TokenType::KEYWORD_VAR_GROUP) {
                customType = token->getText().substr(1); // 去掉 @ 前缀
            } else if (token->getType() == parser::TokenType::IDENTIFIER) {
                customName = token->getText();
            }
        }
    }
    
    // 获取映射后的名称
    std::string mappedName = getMappedName(customName);
    
    // 根据类型创建相应的自定义定义
    if (customType == "Style") {
        auto customStyle = std::make_shared<CustomStyleGroup>(mappedName);
        // TODO: 解析自定义样式内容
        customStyles[mappedName] = customStyle;
    } else if (customType == "Element") {
        auto customElement = std::make_shared<CustomElement>(mappedName);
        // TODO: 解析自定义元素内容
        customElements[mappedName] = customElement;
    } else if (customType == "Var") {
        auto customVar = std::make_shared<CustomVarGroup>(mappedName);
        // TODO: 解析自定义变量内容
        customVars[mappedName] = customVar;
    }
}

// 获取映射后的名称
std::string CHTLImportVisitorStandalone::getMappedName(const std::string& originalName) const {
    auto it = nameMapping_.find(originalName);
    if (it != nameMapping_.end()) {
        return it->second;
    }
    return originalName;
}

} // namespace chtl