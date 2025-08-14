#include "CHTLCodeGenVisitor.h"
#include <algorithm>
#include <regex>

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
    
    // 清空模板存储
    styleTemplates_.clear();
    elementTemplates_.clear();
    varTemplates_.clear();
    localStyleMapping_.clear();
    localStyleCounter_ = 0;
    
    // 开始访问
    if (tree->getName() == "compilationUnit") {
        visitCompilationUnit(tree);
    }
    
    // 生成 CHTL JS 代码
    generateCHTLJS();
    
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
        output << "// CHTL JS Runtime\n";
        output << "const CHTL = {\n";
        output << "    select: function(selector) {\n";
        output << "        const element = selector.startsWith('#') ? \n";
        output << "            document.querySelector(selector) : \n";
        output << "            document.querySelectorAll(selector);\n";
        output << "        \n";
        output << "        // 如果是单个元素，直接返回增强的元素\n";
        output << "        if (selector.startsWith('#') && element) {\n";
        output << "            const enhancedElement = Object.create(element);\n";
        output << "            enhancedElement.listen = function(handlers) {\n";
        output << "                if (typeof handlers === 'object') {\n";
        output << "                    for (const event in handlers) {\n";
        output << "                        this.addEventListener(event, handlers[event]);\n";
        output << "                    }\n";
        output << "                }\n";
        output << "                return this;\n";
        output << "            };\n";
        output << "            return enhancedElement;\n";
        output << "        }\n";
        output << "        \n";
        output << "        // 多个元素的情况\n";
        output << "        return {\n";
        output << "            elements: element,\n";
        output << "            listen: function(handlers) {\n";
        output << "                if (typeof handlers === 'object') {\n";
        output << "                    element.forEach(el => {\n";
        output << "                        for (const event in handlers) {\n";
        output << "                            el.addEventListener(event, handlers[event]);\n";
        output << "                        }\n";
        output << "                    });\n";
        output << "                }\n";
        output << "                return this;\n";
        output << "            },\n";
        output << "            animate: function(keyframes, options) {\n";
        output << "                element.forEach(el => el.animate(keyframes, options));\n";
        output << "                return this;\n";
        output << "            }\n";
        output << "        };\n";
        output << "    }\n";
        output << "};\n\n";
        output << js_.str();
        output << "</script>\n";
    }
    
    output << "</html>\n";
    
    return output.str();
}

void CHTLCodeGenVisitor::visitCompilationUnit(std::shared_ptr<ParseContext> ctx) {
    if (!ctx) return;
    
    // 第一遍：收集所有模板和自定义定义
    for (const auto& child : ctx->getChildren()) {
        if (!child) continue;
        
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx) {
            if (childCtx->getName() == "templateDefinition") {
                visitTemplateDefinition(childCtx);
            } else if (childCtx->getName() == "customDefinition") {
                visitCustomDefinition(childCtx);
            }
        }
    }
    
    // 第二遍：处理其他内容
    for (const auto& child : ctx->getChildren()) {
        if (!child) continue;
        
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx) {
            const std::string& name = childCtx->getName();
            
            if (name == "htmlElement") {
                visitHtmlElement(childCtx);
            } else if (name == "importStatement") {
                visitImportStatement(childCtx);
            } else if (name == "originDeclaration") {
                visitOriginDeclaration(childCtx);
            }
        }
    }
}

void CHTLCodeGenVisitor::visitTemplateDefinition(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().size() < 2) return;
    
    // 第二个子节点应该是具体的模板类型
    auto templateTypeCtx = std::dynamic_pointer_cast<ParseContext>(ctx->getChildren()[1]);
    if (!templateTypeCtx) return;
    
    const std::string& templateType = templateTypeCtx->getName();
    
    if (templateType == "templateStyleGroup") {
        visitStyleTemplate(templateTypeCtx);
    } else if (templateType == "templateElement") {
        visitElementTemplate(templateTypeCtx);
    } else if (templateType == "templateVarGroup") {
        visitVarTemplate(templateTypeCtx);
    }
}

void CHTLCodeGenVisitor::visitStyleTemplate(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().size() < 2) return;
    
    // 获取模板名称
    std::string templateName;
    auto nameNode = ctx->getChildren()[1];
    if (nameNode && nameNode->isTerminal()) {
        templateName = nameNode->getText();
    }
    
    if (templateName.empty()) return;
    
    StyleTemplate& styleTemplate = styleTemplates_[templateName];
    
    // 处理继承
    if (ctx->getChildren().size() > 3 && ctx->getChildren()[2]->getText() == ":") {
        auto baseNode = ctx->getChildren()[3];
        if (baseNode && baseNode->isTerminal()) {
            styleTemplate.baseTemplate = baseNode->getText();
        }
    }
    
    // 收集样式属性
    for (size_t i = 2; i < ctx->getChildren().size(); ++i) {
        auto child = ctx->getChildren()[i];
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx && childCtx->getName() == "styleProperty") {
            // 提取属性名和值
            if (childCtx->getChildren().size() >= 2) {
                std::string propName = childCtx->getChildren()[0]->getText();
                std::string propValue;
                
                // 处理值（可能是字符串、数字或 ThemeColor）
                for (size_t j = 1; j < childCtx->getChildren().size(); ++j) {
                    auto valueNode = childCtx->getChildren()[j];
                    if (valueNode->getText() == "ThemeColor") {
                        // ThemeColor(varName) 处理
                        if (j + 2 < childCtx->getChildren().size()) {
                            std::string varName = childCtx->getChildren()[j + 2]->getText();
                            propValue = resolveVariable(varName);
                            j += 3; // 跳过 '(' varName ')'
                        }
                    } else if (valueNode->getText() != ":" && valueNode->getText() != ";") {
                        propValue += valueNode->getText();
                    }
                }
                
                // 去除字符串引号
                if (propValue.length() >= 2 && propValue.front() == '"' && propValue.back() == '"') {
                    propValue = propValue.substr(1, propValue.length() - 2);
                }
                
                styleTemplate.properties[propName] = propValue;
            }
        }
    }
}

void CHTLCodeGenVisitor::visitElementTemplate(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().size() < 2) return;
    
    // 获取模板名称
    std::string templateName;
    auto nameNode = ctx->getChildren()[1];
    if (nameNode && nameNode->isTerminal()) {
        templateName = nameNode->getText();
    }
    
    if (templateName.empty()) return;
    
    // 存储元素内容
    elementTemplates_[templateName].content = ctx;
}

void CHTLCodeGenVisitor::visitVarTemplate(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().size() < 2) return;
    
    // 获取模板名称
    std::string templateName;
    auto nameNode = ctx->getChildren()[1];
    if (nameNode && nameNode->isTerminal()) {
        templateName = nameNode->getText();
    }
    
    if (templateName.empty()) return;
    
    VarTemplate& varTemplate = varTemplates_[templateName];
    
    // 收集变量定义
    for (size_t i = 2; i < ctx->getChildren().size(); ++i) {
        auto child = ctx->getChildren()[i];
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx && childCtx->getName() == "varDefinition") {
            // 提取变量名和值
            if (childCtx->getChildren().size() >= 3) {
                std::string varName = childCtx->getChildren()[0]->getText();
                std::string varValue = childCtx->getChildren()[2]->getText();
                
                // 去除字符串引号
                if (varValue.length() >= 2 && varValue.front() == '"' && varValue.back() == '"') {
                    varValue = varValue.substr(1, varValue.length() - 2);
                }
                
                varTemplate.variables[varName] = varValue;
            }
        }
    }
}

void CHTLCodeGenVisitor::visitCustomDefinition(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().size() < 2) return;
    
    // 第二个子节点应该是具体的自定义类型
    auto customTypeCtx = std::dynamic_pointer_cast<ParseContext>(ctx->getChildren()[1]);
    if (!customTypeCtx) return;
    
    const std::string& customType = customTypeCtx->getName();
    
    // 自定义和模板共享相同的存储结构
    // 但自定义支持特例化操作
    if (customType == "customStyleGroup") {
        visitStyleTemplate(customTypeCtx); // 复用模板处理逻辑
    } else if (customType == "customElement") {
        visitElementTemplate(customTypeCtx); // 复用模板处理逻辑
    } else if (customType == "customVarGroup") {
        visitVarTemplate(customTypeCtx); // 复用模板处理逻辑
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
    
    // 收集属性和局部样式
    std::vector<std::pair<std::string, std::string>> attributes;
    std::string elementId;
    std::set<std::string> elementClasses;
    bool hasLocalStyle = false;
    
    // 处理子元素
    for (size_t i = 1; i < children.size(); ++i) {
        auto child = children[i];
        if (!child) continue;
        
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx) {
            const std::string& name = childCtx->getName();
            
            if (name == "attribute") {
                // 收集属性
                if (childCtx->getChildren().size() >= 3) {
                    std::string attrName = childCtx->getChildren()[0]->getText();
                    std::string attrValue = childCtx->getChildren()[2]->getText();
                    
                    // 去除引号
                    if (attrValue.length() >= 2 && attrValue.front() == '"' && attrValue.back() == '"') {
                        attrValue = attrValue.substr(1, attrValue.length() - 2);
                    }
                    
                    if (attrName == "id") {
                        elementId = attrValue;
                    } else if (attrName == "class") {
                        elementClasses.insert(attrValue);
                    } else {
                        attributes.push_back({attrName, attrValue});
                    }
                }
            } else if (name == "localStyleBlock") {
                hasLocalStyle = true;
                visitLocalStyleBlock(childCtx, tagName);
            } else if (name == "localScriptBlock") {
                visitLocalScriptBlock(childCtx);
            }
        }
    }
    
    // 如果有局部样式，添加生成的类名
    if (hasLocalStyle) {
        std::string localClass = localStyleMapping_[tagName];
        if (!localClass.empty()) {
            elementClasses.insert(localClass);
        }
    }
    
    // 输出属性
    if (!elementId.empty()) {
        html_ << " id=\"" << elementId << "\"";
    }
    
    if (!elementClasses.empty()) {
        html_ << " class=\"";
        bool first = true;
        for (const auto& cls : elementClasses) {
            if (!first) html_ << " ";
            html_ << cls;
            first = false;
        }
        html_ << "\"";
    }
    
    for (const auto& attr : attributes) {
        html_ << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    html_ << ">";
    
    // 处理内容
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
            } else if (name.find("Template") != std::string::npos) {
                // 处理模板使用
                if (child->isTerminal()) {
                    std::string templateName = child->getText();
                    if (name == "styleTemplate") {
                        applyStyleTemplate(templateName);
                    } else if (name == "elementTemplate") {
                        applyElementTemplate(templateName);
                    }
                }
            }
        }
    }
    
    // 生成结束标签
    html_ << "</" << tagName << ">";
}

void CHTLCodeGenVisitor::visitLocalStyleBlock(std::shared_ptr<ParseContext> ctx, const std::string& elementTag) {
    if (!ctx) return;
    
    // 生成唯一的类名
    std::string className = generateUniqueClassName();
    localStyleMapping_[elementTag] = className;
    
    // 开始 CSS 规则
    css_ << "." << className << " {\n";
    
    // 处理样式属性
    for (const auto& child : ctx->getChildren()) {
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx && childCtx->getName() == "styleProperty") {
            if (childCtx->getChildren().size() >= 2) {
                std::string propName = childCtx->getChildren()[0]->getText();
                std::string propValue;
                
                for (size_t j = 1; j < childCtx->getChildren().size(); ++j) {
                    auto valueNode = childCtx->getChildren()[j];
                    if (valueNode->getText() != ":" && valueNode->getText() != ";") {
                        propValue += valueNode->getText();
                    }
                }
                
                // 去除引号
                if (propValue.length() >= 2 && propValue.front() == '"' && propValue.back() == '"') {
                    propValue = propValue.substr(1, propValue.length() - 2);
                }
                
                css_ << "    " << propName << ": " << propValue << ";\n";
            }
        } else if (childCtx && (childCtx->getName() == "classSelector" || 
                                childCtx->getName() == "idSelector")) {
            // 处理嵌套选择器
            std::string selector;
            if (childCtx->getName() == "classSelector") {
                selector = "." + childCtx->getChildren()[0]->getText();
            } else {
                selector = "#" + childCtx->getChildren()[0]->getText();
            }
            
            css_ << "}\n";
            css_ << "." << className << " " << selector << " {\n";
            
            // 处理选择器内的属性
            for (size_t i = 1; i < childCtx->getChildren().size(); ++i) {
                auto propCtx = std::dynamic_pointer_cast<ParseContext>(childCtx->getChildren()[i]);
                if (propCtx && propCtx->getName() == "styleProperty") {
                    visitStyleProperty(propCtx);
                }
            }
        }
    }
    
    css_ << "}\n\n";
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

void CHTLCodeGenVisitor::visitStyleProperty(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().size() < 2) return;
    
    std::string propName = ctx->getChildren()[0]->getText();
    std::string propValue;
    
    for (size_t i = 1; i < ctx->getChildren().size(); ++i) {
        auto valueNode = ctx->getChildren()[i];
        if (valueNode->getText() != ":" && valueNode->getText() != ";") {
            propValue += valueNode->getText();
        }
    }
    
    // 去除引号
    if (propValue.length() >= 2 && propValue.front() == '"' && propValue.back() == '"') {
        propValue = propValue.substr(1, propValue.length() - 2);
    }
    
    css_ << "    " << propName << ": " << propValue << ";\n";
}

void CHTLCodeGenVisitor::visitLocalScriptBlock(std::shared_ptr<ParseContext> ctx) {
    if (!ctx) return;
    
    // 处理脚本内容，转换 CHTL JS 语法
    std::string scriptContent;
    
    for (const auto& child : ctx->getChildren()) {
        if (child && child->isTerminal()) {
            std::string text = child->getText();
            // 转换 CHTL JS 语法
            text = transformCHTLJS(text);
            scriptContent += text;
        }
    }
    
    js_ << scriptContent << "\n";
}

std::string CHTLCodeGenVisitor::transformCHTLJS(const std::string& code) {
    std::string result = code;
    
    // 转换 {{selector}} 语法
    std::regex selectorRegex(R"(\{\{([^\}]+)\}\})");
    result = std::regex_replace(result, selectorRegex, "CHTL.select('$1')");
    
    // 转换 -> 为 .
    size_t pos = 0;
    while ((pos = result.find("->", pos)) != std::string::npos) {
        result.replace(pos, 2, ".");
        pos += 1;
    }
    
    return result;
}

void CHTLCodeGenVisitor::visitOriginDeclaration(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().empty()) return;
    
    // 第一个子节点应该是原始嵌入的类型
    auto typeCtx = std::dynamic_pointer_cast<ParseContext>(ctx->getChildren()[0]);
    if (!typeCtx) return;
    
    const std::string& originType = typeCtx->getName();
    
    // 获取原始内容
    std::string content;
    if (typeCtx->getChildren().size() > 0) {
        auto contentNode = typeCtx->getChildren()[0];
        if (contentNode && contentNode->isTerminal()) {
            content = contentNode->getText();
        }
    }
    
    // 根据类型输出到相应的流
    if (originType == "originHtml") {
        html_ << content;
    } else if (originType == "originStyle") {
        css_ << content << "\n";
    } else if (originType == "originJavaScript") {
        js_ << content << "\n";
    }
}

void CHTLCodeGenVisitor::visitImportStatement(std::shared_ptr<ParseContext> ctx) {
    if (!ctx) return;
    
    // 解析导入语句
    auto importCtx = std::dynamic_pointer_cast<ParseContext>(ctx->getChildren()[0]);
    if (!importCtx) return;
    
    const std::string& importType = importCtx->getName();
    
    if (importType == "fileImport") {
        // 处理文件导入
        handleFileImport(importCtx);
    } else if (importType == "specificImport") {
        // 处理特定导入
        handleSpecificImport(importCtx);
    }
}

void CHTLCodeGenVisitor::handleFileImport(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().size() < 2) return;
    
    // 获取文件类型和路径
    std::string fileType = ctx->getChildren()[0]->getText();
    std::string filePath = ctx->getChildren()[1]->getText();
    
    // 去除路径的引号
    if (filePath.length() >= 2 && filePath.front() == '"' && filePath.back() == '"') {
        filePath = filePath.substr(1, filePath.length() - 2);
    }
    
    // 检查是否有 as 子句
    std::string asName;
    for (size_t i = 2; i < ctx->getChildren().size(); ++i) {
        if (ctx->getChildren()[i]->getText() == "as" && i + 1 < ctx->getChildren().size()) {
            asName = ctx->getChildren()[i + 1]->getText();
            break;
        }
    }
    
    // 如果有 as 子句，创建命名的原始嵌入节点
    if (!asName.empty()) {
        if (fileType == "Html") {
            // 将导入的HTML内容存储为命名节点，供后续使用
            // TODO: 实际读取文件内容
            namedOriginNodes_[asName] = {fileType, "<!-- Content from " + filePath + " -->"};
        } else if (fileType == "Style") {
            namedOriginNodes_[asName] = {fileType, "/* Content from " + filePath + " */"};
        } else if (fileType == "JavaScript") {
            namedOriginNodes_[asName] = {fileType, "// Content from " + filePath};
        }
    }
    
    // 如果没有 as 子句，根据CHTL语法文档，直接跳过
}

void CHTLCodeGenVisitor::handleSpecificImport(std::shared_ptr<ParseContext> ctx) {
    // TODO: 处理特定导入（模板、自定义等）
    (void)ctx;
}

void CHTLCodeGenVisitor::applyStyleTemplate(const std::string& templateName) {
    auto it = styleTemplates_.find(templateName);
    if (it == styleTemplates_.end()) return;
    
    const StyleTemplate& styleTemplate = it->second;
    
    // 应用基础模板（如果有）
    if (!styleTemplate.baseTemplate.empty()) {
        applyStyleTemplate(styleTemplate.baseTemplate);
    }
    
    // 应用当前模板的属性
    for (const auto& prop : styleTemplate.properties) {
        css_ << "    " << prop.first << ": " << prop.second << ";\n";
    }
}

void CHTLCodeGenVisitor::applyElementTemplate(const std::string& templateName) {
    auto it = elementTemplates_.find(templateName);
    if (it == elementTemplates_.end()) return;
    
    // 访问模板内容
    auto templateCtx = it->second.content;
    if (templateCtx) {
        // 从模板内容中提取元素
        for (size_t i = 2; i < templateCtx->getChildren().size(); ++i) {
            auto child = templateCtx->getChildren()[i];
            auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
            if (childCtx && childCtx->getName() == "htmlElement") {
                visitHtmlElement(childCtx);
            }
        }
    }
}

std::string CHTLCodeGenVisitor::resolveVariable(const std::string& varName) {
    // 在所有变量组中查找
    for (const auto& varTemplate : varTemplates_) {
        auto it = varTemplate.second.variables.find(varName);
        if (it != varTemplate.second.variables.end()) {
            return it->second;
        }
    }
    
    // 如果找不到，返回原始变量名
    return "var(--" + varName + ")";
}

void CHTLCodeGenVisitor::generateCHTLJS() {
    // CHTL JS 运行时已经在 visit() 方法中生成
    // 这里可以添加页面特定的 JS 代码
}

std::string CHTLCodeGenVisitor::generateDOMSelector(const std::string& selector) {
    return "CHTL.select('" + selector + "')";
}

std::string CHTLCodeGenVisitor::generateChainAccess(const std::string& base, const std::string& member) {
    return base + ".chain('" + member + "')";
}

std::string CHTLCodeGenVisitor::generateEventListener(const std::string& selector, 
                                                      const std::string& event, 
                                                      const std::string& handler) {
    return generateDOMSelector(selector) + ".listen('" + event + "', " + handler + ")";
}

std::string CHTLCodeGenVisitor::generateAnimation(const std::string& selector, 
                                                 const std::string& animation) {
    return generateDOMSelector(selector) + ".animate(" + animation + ")";
}

std::string CHTLCodeGenVisitor::getNodeText(std::shared_ptr<ParseTree> node) {
    if (!node) return "";
    return node->getText();
}

void CHTLCodeGenVisitor::visitChildren(std::shared_ptr<ParseContext> ctx) {
    if (!ctx) return;
    
    for (const auto& child : ctx->getChildren()) {
        if (!child) continue;
        
        auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
        if (childCtx) {
            visit(childCtx);
        }
    }
}

std::string CHTLCodeGenVisitor::generateUniqueClassName() {
    return "chtl-local-" + std::to_string(++localStyleCounter_);
}

} // namespace parser
} // namespace chtl