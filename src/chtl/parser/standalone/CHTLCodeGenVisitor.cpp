#include "CHTLCodeGenVisitor.h"
#include <algorithm>
#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>

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
            } else if (name == "originDeclaration") {
                visitOriginDeclaration(childCtx);
            } else if (name == "exceptConstraint") {
                visitExceptConstraint(childCtx);
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

void CHTLCodeGenVisitor::visitExceptConstraint(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().empty()) return;
    
    // except关键字已经被解析器消费
    // 我们需要收集约束目标并生成相应的注释或约束代码
    
    std::stringstream exceptStatement;
    exceptStatement << "except ";
    
    bool first = true;
    for (size_t i = 1; i < ctx->getChildren().size(); ++i) {
        auto child = ctx->getChildren()[i];
        if (!child) continue;
        
        if (!first) {
            exceptStatement << ", ";
        }
        
        // 获取约束目标的文本
        if (child->isTerminal()) {
            exceptStatement << child->getText();
        } else {
            auto childCtx = std::dynamic_pointer_cast<ParseContext>(child);
            if (childCtx) {
                // 递归构建约束文本
                for (auto grandChild : childCtx->getChildren()) {
                    if (grandChild && grandChild->isTerminal()) {
                        exceptStatement << grandChild->getText() << " ";
                    }
                }
            }
        }
        
        first = false;
    }
    
    // 生成HTML注释来标记约束
    html_ << "<!-- CHTL Constraint: " << exceptStatement.str() << " -->\n";
    
    // TODO: 与CHTLGenerator的约束管理器集成
    // 这里应该调用generator的processExceptStatement方法
    // 但由于这是独立的代码生成器，我们暂时只生成注释
}

void CHTLCodeGenVisitor::visitOriginDeclaration(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().empty()) return;
    

    
    // 第一个子节点应该是原始嵌入的类型
    auto typeCtx = std::dynamic_pointer_cast<ParseContext>(ctx->getChildren()[0]);
    if (!typeCtx) return;
    
    const std::string& originType = typeCtx->getName();
    
    // 检查是否是命名引用（通过查看typeCtx的内容）
    std::string typeCtxText = typeCtx->getText();
    
    // 如果内容包含分号，说明是引用
    bool isNamedReference = typeCtxText.find(";") != std::string::npos;
    
    if (isNamedReference) {
        // 从typeCtx的文本中提取引用名称
        // 格式是: "@Html helloContent ;"
        std::string referenceName;
        size_t atPos = typeCtxText.find("@");
        if (atPos != std::string::npos) {
            size_t spacePos = typeCtxText.find(" ", atPos);
            if (spacePos != std::string::npos) {
                size_t endPos = typeCtxText.find(" ", spacePos + 1);
                if (endPos != std::string::npos) {
                    referenceName = typeCtxText.substr(spacePos + 1, endPos - spacePos - 1);
                } else {
                    size_t semicolonPos = typeCtxText.find(";");
                    if (semicolonPos != std::string::npos) {
                        referenceName = typeCtxText.substr(spacePos + 1, semicolonPos - spacePos - 1);
                    }
                }
            }
        }
        

        
        // 从namedOriginNodes_中查找内容
        if (!referenceName.empty()) {
            auto it = namedOriginNodes_.find(referenceName);
            if (it != namedOriginNodes_.end()) {
                const auto& [type, content] = it->second;

                if (type == "Html") {
                    html_ << content;
                } else if (type == "Style") {
                    css_ << content << "\n";
                } else if (type == "JavaScript") {
                    js_ << content << "\n";
                }
            } else {
                std::cerr << "Warning: Named origin node '" << referenceName << "' not found\n";
            }
        }
    } else {
        // 获取原始内容（原有逻辑）
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
}

void CHTLCodeGenVisitor::visitImportStatement(std::shared_ptr<ParseContext> ctx) {
    if (!ctx) return;
    
    // 打印整个导入语句的文本
    std::cerr << "Import statement full text: " << ctx->getText() << "\n";
    

    
    // 检查是什么类型的导入
    // 由于解析器可能合并了节点，我们需要在源文本中查找
    std::string fullText = ctx->getText();
    
    // 查找是否包含 [Template] 或 [Custom] 标记
    bool isSpecificImport = false;
    
    // 检查第二个子节点
    if (ctx->getChildren().size() > 1) {
        auto secondChild = ctx->getChildren()[1];
        if (secondChild) {
            std::string childText = secondChild->getText();
            // 检查是否以Template或Custom开头
            if (childText.find("Template") == 0 || childText.find("Custom") == 0) {
                isSpecificImport = true;
            }
        }
    }
    
    if (isSpecificImport) {
        std::cerr << "Processing specific import\n";
        handleSpecificImport(ctx);
    } else {
        std::cerr << "Processing file import\n";
        handleFileImport(ctx);
    }
}

void CHTLCodeGenVisitor::handleFileImport(std::shared_ptr<ParseContext> ctx) {
    if (!ctx || ctx->getChildren().size() < 2) return;
    

    
    // 第二个子节点应该是fileImport
    auto fileImportCtx = std::dynamic_pointer_cast<ParseContext>(ctx->getChildren()[1]);
    if (!fileImportCtx) return;
    

    
    // 解析fileImport内容
    // 文本格式看起来是: Html"hello.html"helloContent (没有空格)
    std::string fileImportText = fileImportCtx->getText();
    
    // 提取文件类型 (从开头到第一个引号)
    std::string fileType;
    size_t firstQuote = fileImportText.find("\"");
    if (firstQuote > 0) {
        fileType = fileImportText.substr(0, firstQuote);
        // 去掉@符号
        if (fileType.find("@") == 0) {
            fileType = fileType.substr(1);
        }
    }
    
    // 提取文件路径
    std::string filePath;
    if (firstQuote != std::string::npos) {
        size_t secondQuote = fileImportText.find("\"", firstQuote + 1);
        if (secondQuote != std::string::npos) {
            filePath = fileImportText.substr(firstQuote + 1, secondQuote - firstQuote - 1);
        }
    }
    
    // 提取as名称 (第二个引号之后的部分)
    std::string asName;
    size_t secondQuote = fileImportText.find("\"", firstQuote + 1);
    if (secondQuote != std::string::npos && secondQuote + 1 < fileImportText.length()) {
        asName = fileImportText.substr(secondQuote + 1);
        // 去除末尾空格和换行
        asName.erase(asName.find_last_not_of(" \n\r\t") + 1);
    }
    

    
    // 如果有 as 子句，创建命名的原始嵌入节点
    if (!asName.empty()) {
        // 读取文件内容
        std::string fileContent;
        std::ifstream file(filePath);
        
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            fileContent = buffer.str();
            file.close();
        } else {
            // 如果文件不存在，记录错误并使用占位内容
            std::cerr << "Warning: Cannot open file: " + filePath << std::endl;
            fileContent = "<!-- Error: Cannot read file " + filePath + " -->";
        }
        
        if (fileType == "Html") {
            // 将导入的HTML内容存储为命名节点，供后续使用
            namedOriginNodes_[asName] = {fileType, fileContent};
        } else if (fileType == "Style") {
            namedOriginNodes_[asName] = {fileType, fileContent};
        } else if (fileType == "JavaScript") {
            namedOriginNodes_[asName] = {fileType, fileContent};
        }
    }
    
    // 如果没有 as 子句，根据CHTL语法文档，直接跳过
}

void CHTLCodeGenVisitor::handleSpecificImport(std::shared_ptr<ParseContext> ctx) {
    if (!ctx) return;
    
    // 解析特定导入语句
    // 格式: [Import] [Template/Custom] @Type name from "file" as alias
    
    std::string importCategory;  // Template 或 Custom
    std::string importType;      // Element, Style, 或 Var
    std::string itemName;        // 要导入的项目名称
    std::string filePath;        // 源文件路径
    std::string asName;          // 可选的别名
    
    // 遍历子节点以提取信息
    bool foundCategory = false;
    bool foundType = false;
    bool foundFrom = false;
    
    for (size_t i = 0; i < ctx->getChildren().size(); ++i) {
        auto child = ctx->getChildren()[i];
        if (!child) continue;
        
        std::string text = child->getText();
        
        // 查找 [Template] 或 [Custom]
        if (text == "[Template]" || text == "Template") {
            importCategory = "Template";
            foundCategory = true;
        } else if (text == "[Custom]" || text == "Custom") {
            importCategory = "Custom";
            foundCategory = true;
        }
        // 查找 @Element, @Style, @Var
        else if (text.find("@") == 0) {
            importType = text.substr(1);  // 去掉@符号
            foundType = true;
            // 下一个应该是项目名称
            if (i + 1 < ctx->getChildren().size()) {
                itemName = ctx->getChildren()[i + 1]->getText();
                i++;  // 跳过已处理的名称
            }
        }
        // 查找 from 关键字
        else if (text == "from" && i + 1 < ctx->getChildren().size()) {
            foundFrom = true;
            filePath = ctx->getChildren()[i + 1]->getText();
            // 去除引号
            if (filePath.length() >= 2 && filePath.front() == '"' && filePath.back() == '"') {
                filePath = filePath.substr(1, filePath.length() - 2);
            }
            i++;  // 跳过文件路径
        }
        // 查找 as 关键字
        else if (text == "as" && i + 1 < ctx->getChildren().size()) {
            asName = ctx->getChildren()[i + 1]->getText();
            i++;  // 跳过别名
        }
    }
    
    // 验证必要的信息是否完整
    if (!foundCategory || !foundType || !foundFrom || itemName.empty() || filePath.empty()) {
        std::cerr << "Warning: Incomplete specific import statement\n";
        return;
    }
    
    // 读取并解析目标文件
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Warning: Cannot open file for specific import: " << filePath << std::endl;
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();
    file.close();
    
    // 根据导入类型处理
    if (importCategory == "Template") {
        if (importType == "Element") {
            // TODO: 解析文件内容，查找指定的模板元素定义
            // 暂时存储为注释
            std::string templateDef = "<!-- Template Element '" + itemName + "' from " + filePath + " -->";
            
            // 如果有别名，使用别名；否则使用原始名称
            std::string storeName = asName.empty() ? itemName : asName;
            // elementTemplates_ 需要 ElementTemplate 类型
            ElementTemplate elemTemplate;
            // 创建一个虚拟的 ParseContext
            elemTemplate.content = std::make_shared<ParseContext>("importedElement");
            elementTemplates_[storeName] = elemTemplate;
            
            std::cerr << "Imported Template Element: " << itemName << " as " << storeName << std::endl;
        } else if (importType == "Style") {
            // TODO: 解析文件内容，查找指定的模板样式定义
            std::string templateDef = "/* Template Style '" + itemName + "' from " + filePath + " */";
            
            std::string storeName = asName.empty() ? itemName : asName;
            // styleTemplates_ 需要 StyleTemplate 类型
            StyleTemplate styleTemplate;
            // 暂时存储为属性
            styleTemplate.properties["_import_content"] = templateDef;
            styleTemplates_[storeName] = styleTemplate;
            
            std::cerr << "Imported Template Style: " << itemName << " as " << storeName << std::endl;
        } else if (importType == "Var") {
            // TODO: 解析文件内容，查找指定的模板变量定义
            std::string templateDef = "/* Template Var '" + itemName + "' from " + filePath + " */";
            
            std::string storeName = asName.empty() ? itemName : asName;
            // varTemplates_ 需要 VarTemplate 类型，暂时创建一个简单的
            VarTemplate varTemplate;
            // 暂时存储为变量
            varTemplate.variables["_import_content"] = templateDef;
            varTemplates_[storeName] = varTemplate;
            
            std::cerr << "Imported Template Var: " << itemName << " as " << storeName << std::endl;
        }
    } else if (importCategory == "Custom") {
        if (importType == "Element") {
            // TODO: 解析文件内容，查找指定的自定义元素定义
            std::string customDef = "<!-- Custom Element '" + itemName + "' from " + filePath + " -->";
            
            std::string storeName = asName.empty() ? itemName : asName;
            customElements_[storeName] = std::make_shared<ParseContext>("customElement");
            
            std::cerr << "Imported Custom Element: " << itemName << " as " << storeName << std::endl;
        } else if (importType == "Style") {
            // TODO: 解析文件内容，查找指定的自定义样式定义
            std::string customDef = "/* Custom Style '" + itemName + "' from " + filePath + " */";
            
            std::string storeName = asName.empty() ? itemName : asName;
            customStyles_[storeName] = customDef;
            
            std::cerr << "Imported Custom Style: " << itemName << " as " << storeName << std::endl;
        } else if (importType == "Var") {
            // TODO: 解析文件内容，查找指定的自定义变量定义
            std::string customDef = "/* Custom Var '" + itemName + "' from " + filePath + " */";
            
            std::string storeName = asName.empty() ? itemName : asName;
            customVars_[storeName] = customDef;
            
            std::cerr << "Imported Custom Var: " << itemName << " as " << storeName << std::endl;
        }
    }
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