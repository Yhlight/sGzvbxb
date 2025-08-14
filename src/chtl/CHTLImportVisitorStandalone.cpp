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
        
        // 解析样式内容
        // 查找包含样式属性的块
        for (const auto& child : ctx->getChildren()) {
            if (auto blockCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
                if (blockCtx->getName() == "styleBlock" || 
                    blockCtx->getName() == "block") {
                    // 解析样式属性
                    parseStyleProperties(blockCtx, styleTemplate);
                    break;
                }
            }
        }
        
        templates[mappedName] = styleTemplate;
    } else if (templateType == "Element") {
        auto elementTemplate = std::make_shared<ElementTemplate>(mappedName);
        
        // 解析元素内容
        for (const auto& child : ctx->getChildren()) {
            if (auto blockCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
                if (blockCtx->getName() == "elementBlock" || 
                    blockCtx->getName() == "block") {
                    parseElementContent(blockCtx, elementTemplate);
                    break;
                }
            }
        }
        
        templates[mappedName] = elementTemplate;
    } else if (templateType == "Var") {
        auto varTemplate = std::make_shared<VarTemplate>(mappedName);
        
        // 解析变量内容
        for (const auto& child : ctx->getChildren()) {
            if (auto blockCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
                if (blockCtx->getName() == "varBlock" || 
                    blockCtx->getName() == "block") {
                    parseVarContent(blockCtx, varTemplate);
                    break;
                }
            }
        }
        
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
        
        // 解析自定义样式内容
        for (const auto& child : ctx->getChildren()) {
            if (auto blockCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
                if (blockCtx->getName() == "styleBlock" || 
                    blockCtx->getName() == "block") {
                    parseCustomStyleContent(blockCtx, customStyle);
                    break;
                }
            }
        }
        
        customStyles[mappedName] = customStyle;
    } else if (customType == "Element") {
        auto customElement = std::make_shared<CustomElement>(mappedName);
        
        // 解析自定义元素内容
        for (const auto& child : ctx->getChildren()) {
            if (auto blockCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
                if (blockCtx->getName() == "elementBlock" || 
                    blockCtx->getName() == "block") {
                    parseCustomElementContent(blockCtx, customElement);
                    break;
                }
            }
        }
        
        customElements[mappedName] = customElement;
    } else if (customType == "Var") {
        auto customVar = std::make_shared<CustomVarGroup>(mappedName);
        
        // 解析自定义变量内容
        for (const auto& child : ctx->getChildren()) {
            if (auto blockCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
                if (blockCtx->getName() == "varBlock" || 
                    blockCtx->getName() == "block") {
                    parseCustomVarContent(blockCtx, customVar);
                    break;
                }
            }
        }
        
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

void CHTLImportVisitorStandalone::parseStyleProperties(std::shared_ptr<parser::ParseContext> ctx,
                                                      std::shared_ptr<StyleTemplate> styleTemplate) {
    if (!ctx || !styleTemplate) return;
    
    // 遍历样式块中的所有子节点
    for (const auto& child : ctx->getChildren()) {
        if (auto propCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
            std::string ruleName = propCtx->getName();
            
            // 处理样式属性
            if (ruleName == "styleProperty" || ruleName == "property") {
                std::string propName;
                std::string propValue;
                
                // 提取属性名和值
                auto children = propCtx->getChildren();
                if (children.size() >= 3) {  // name : value
                    if (auto nameNode = std::dynamic_pointer_cast<parser::TerminalNode>(children[0])) {
                        propName = nameNode->getText();
                    }
                    
                    // 跳过冒号，获取值
                    if (auto valueNode = std::dynamic_pointer_cast<parser::TerminalNode>(children[2])) {
                        propValue = valueNode->getText();
                    } else if (auto valueCtx = std::dynamic_pointer_cast<parser::ParseContext>(children[2])) {
                        // 值可能是复合的
                        propValue = valueCtx->getText();
                    }
                }
                
                // 将属性添加到样式模板
                if (!propName.empty() && !propValue.empty()) {
                    styleTemplate->addStyle(propName, propValue);
                }
            }
            // 处理继承
            else if (ruleName == "inherit" || ruleName == "styleInherit") {
                for (const auto& inheritChild : propCtx->getChildren()) {
                    if (auto inheritNode = std::dynamic_pointer_cast<parser::TerminalNode>(inheritChild)) {
                        std::string text = inheritNode->getText();
                        if (text.find("@Style") != std::string::npos) {
                            // 提取继承的样式名
                            size_t pos = text.find("@Style") + 6;
                            std::string inheritName = text.substr(pos);
                            inheritName.erase(0, inheritName.find_first_not_of(" \t"));
                            inheritName.erase(inheritName.find_last_not_of(" \t;") + 1);
                            
                            styleTemplate->inheritFrom(inheritName);
                        }
                    }
                }
            }
            // 处理嵌套的样式规则
            else if (ruleName == "block" && !propCtx->getChildren().empty()) {
                // 递归解析嵌套块
                parseStyleProperties(propCtx, styleTemplate);
            }
        }
    }
}

void CHTLImportVisitorStandalone::parseElementContent(std::shared_ptr<parser::ParseContext> ctx,
                                                     std::shared_ptr<ElementTemplate> elementTemplate) {
    if (!ctx || !elementTemplate) return;
    
    // 元素模板存储整个内容块
    // TODO: 解析元素结构并添加到elementTemplate
    // elementTemplate->setContent(ctx);
    
    // 可以在这里提取元素的结构信息
    // 比如标签名、属性、子元素等
}

void CHTLImportVisitorStandalone::parseVarContent(std::shared_ptr<parser::ParseContext> ctx,
                                                  std::shared_ptr<VarTemplate> varTemplate) {
    if (!ctx || !varTemplate) return;
    
    // 遍历变量块中的所有子节点
    for (const auto& child : ctx->getChildren()) {
        if (auto varCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
            std::string ruleName = varCtx->getName();
            
            // 处理变量定义
            if (ruleName == "varDefinition" || ruleName == "variable") {
                std::string varName;
                std::string varValue;
                
                // 提取变量名和值
                auto children = varCtx->getChildren();
                if (children.size() >= 3) {  // name : value
                    if (auto nameNode = std::dynamic_pointer_cast<parser::TerminalNode>(children[0])) {
                        varName = nameNode->getText();
                    }
                    
                    // 跳过冒号，获取值
                    if (auto valueNode = std::dynamic_pointer_cast<parser::TerminalNode>(children[2])) {
                        varValue = valueNode->getText();
                    } else if (auto valueCtx = std::dynamic_pointer_cast<parser::ParseContext>(children[2])) {
                        varValue = valueCtx->getText();
                    }
                }
                
                // 将变量添加到变量模板
                if (!varName.empty() && !varValue.empty()) {
                    varTemplate->addVariable(varName, varValue);
                }
            }
            // 处理嵌套的变量定义
            else if (ruleName == "block" && !varCtx->getChildren().empty()) {
                // 递归解析嵌套块
                parseVarContent(varCtx, varTemplate);
            }
        }
    }
}

void CHTLImportVisitorStandalone::parseCustomStyleContent(std::shared_ptr<parser::ParseContext> ctx,
                                                         std::shared_ptr<CustomStyleGroup> customStyle) {
    if (!ctx || !customStyle) return;
    
    // 自定义样式组可以包含样式规则、继承、特化操作等
    for (const auto& child : ctx->getChildren()) {
        if (auto propCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
            std::string ruleName = propCtx->getName();
            
            // 处理样式属性
            if (ruleName == "styleProperty" || ruleName == "property") {
                std::string propName;
                std::string propValue;
                
                auto children = propCtx->getChildren();
                if (children.size() >= 3) {
                    if (auto nameNode = std::dynamic_pointer_cast<parser::TerminalNode>(children[0])) {
                        propName = nameNode->getText();
                    }
                    if (auto valueNode = std::dynamic_pointer_cast<parser::TerminalNode>(children[2])) {
                        propValue = valueNode->getText();
                    } else if (auto valueCtx = std::dynamic_pointer_cast<parser::ParseContext>(children[2])) {
                        propValue = valueCtx->getText();
                    }
                }
                
                if (!propName.empty() && !propValue.empty()) {
                    customStyle->addStyle(propName, propValue);
                }
            }
            // 处理delete操作
            else if (ruleName == "deleteProperty") {
                for (const auto& deleteChild : propCtx->getChildren()) {
                    if (auto deleteNode = std::dynamic_pointer_cast<parser::TerminalNode>(deleteChild)) {
                        std::string text = deleteNode->getText();
                        if (text != "delete" && text != ";") {
                            customStyle->addDeleteOperation({text});
                        }
                    }
                }
            }
            // 处理valueless样式组（只有属性名的列表）
            else if (ruleName == "valuelessProperties") {
                for (const auto& propChild : propCtx->getChildren()) {
                    if (auto propNode = std::dynamic_pointer_cast<parser::TerminalNode>(propChild)) {
                        std::string propName = propNode->getText();
                        if (propName != "," && propName != ";") {
                            customStyle->addValuelessProperty(propName);
                        }
                    }
                }
            }
        }
    }
}

void CHTLImportVisitorStandalone::parseCustomElementContent(std::shared_ptr<parser::ParseContext> ctx,
                                                           std::shared_ptr<CustomElement> customElement) {
    if (!ctx || !customElement) return;
    
    // 自定义元素存储整个定义
    // TODO: 解析自定义元素定义并添加到customElement
    // customElement->setDefinition(ctx);
    
    // 可以提取元素的特定信息
    // 如参数化元素的参数列表等
}

void CHTLImportVisitorStandalone::parseCustomVarContent(std::shared_ptr<parser::ParseContext> ctx,
                                                        std::shared_ptr<CustomVarGroup> customVar) {
    if (!ctx || !customVar) return;
    
    // 遍历变量块中的所有子节点
    for (const auto& child : ctx->getChildren()) {
        if (auto varCtx = std::dynamic_pointer_cast<parser::ParseContext>(child)) {
            std::string ruleName = varCtx->getName();
            
            // 处理变量定义
            if (ruleName == "varDefinition" || ruleName == "variable") {
                std::string varName;
                std::string varValue;
                
                auto children = varCtx->getChildren();
                if (children.size() >= 3) {
                    if (auto nameNode = std::dynamic_pointer_cast<parser::TerminalNode>(children[0])) {
                        varName = nameNode->getText();
                    }
                    if (auto valueNode = std::dynamic_pointer_cast<parser::TerminalNode>(children[2])) {
                        varValue = valueNode->getText();
                    } else if (auto valueCtx = std::dynamic_pointer_cast<parser::ParseContext>(children[2])) {
                        varValue = valueCtx->getText();
                    }
                }
                
                if (!varName.empty() && !varValue.empty()) {
                    customVar->addVariable(varName, varValue);
                }
            }
        }
    }
}

} // namespace chtl