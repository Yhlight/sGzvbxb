#include "AutoClassIdProcessor.h"
#include "parser/standalone/ParserRuntime.h"
#include <sstream>
#include <algorithm>

namespace chtl {

AutoClassIdProcessor::StyleBlockInfo 
AutoClassIdProcessor::processLocalStyleBlock(std::shared_ptr<parser::ParseContext> styleBlock) {
    StyleBlockInfo info;
    
    if (!styleBlock || styleBlock->getName() != "localStyleBlock") {
        return info;
    }
    
    std::stringstream globalCss;
    std::stringstream inlineCss;
    
    // 遍历样式块的子节点
    for (const auto& child : styleBlock->getChildren()) {
        auto childCtx = std::dynamic_pointer_cast<parser::ParseContext>(child);
        if (!childCtx) continue;
        
        const std::string& nodeName = childCtx->getName();
        
        if (nodeName == "classSelector") {
            // 类选择器 - 提取类名并生成全局CSS
            auto firstChild = childCtx->getChildren().front();
            if (auto terminal = std::dynamic_pointer_cast<parser::TerminalNode>(firstChild)) {
                std::string className = terminal->getToken()->getText();
                info.autoClasses.insert(className);
                
                // 收集CSS属性
                std::vector<std::pair<std::string, std::string>> properties;
                for (size_t i = 1; i < childCtx->getChildren().size(); ++i) {
                    auto prop = std::dynamic_pointer_cast<parser::ParseContext>(childCtx->getChildren()[i]);
                    if (prop && prop->getName() == "styleProperty") {
                        // 解析属性
                        if (prop->getChildren().size() >= 2) {
                            auto propName = std::dynamic_pointer_cast<parser::TerminalNode>(prop->getChildren()[0]);
                            auto propValue = std::dynamic_pointer_cast<parser::TerminalNode>(prop->getChildren()[1]);
                            if (propName && propValue) {
                                properties.push_back({
                                    propName->getToken()->getText(),
                                    propValue->getToken()->getText()
                                });
                            }
                        }
                    }
                }
                
                // 生成CSS规则
                globalCss << generateCssRule("." + className, properties) << "\n";
            }
        }
        else if (nodeName == "idSelector") {
            // ID选择器 - 提取ID并生成全局CSS
            auto firstChild = childCtx->getChildren().front();
            if (auto terminal = std::dynamic_pointer_cast<parser::TerminalNode>(firstChild)) {
                std::string idName = terminal->getToken()->getText();
                info.autoIds.insert(idName);
                
                // 收集CSS属性
                std::vector<std::pair<std::string, std::string>> properties;
                for (size_t i = 1; i < childCtx->getChildren().size(); ++i) {
                    auto prop = std::dynamic_pointer_cast<parser::ParseContext>(childCtx->getChildren()[i]);
                    if (prop && prop->getName() == "styleProperty") {
                        // 解析属性
                        if (prop->getChildren().size() >= 2) {
                            auto propName = std::dynamic_pointer_cast<parser::TerminalNode>(prop->getChildren()[0]);
                            auto propValue = std::dynamic_pointer_cast<parser::TerminalNode>(prop->getChildren()[1]);
                            if (propName && propValue) {
                                properties.push_back({
                                    propName->getToken()->getText(),
                                    propValue->getToken()->getText()
                                });
                            }
                        }
                    }
                }
                
                // 生成CSS规则
                globalCss << generateCssRule("#" + idName, properties) << "\n";
            }
        }
        else if (nodeName == "contextSelector") {
            // & 上下文选择器 - 需要在生成时处理
            // 暂时跳过，需要知道当前元素的类名/ID
        }
        else if (nodeName == "styleProperty") {
            // 内联样式属性
            if (childCtx->getChildren().size() >= 2) {
                auto propName = std::dynamic_pointer_cast<parser::TerminalNode>(childCtx->getChildren()[0]);
                auto propValue = std::dynamic_pointer_cast<parser::TerminalNode>(childCtx->getChildren()[1]);
                if (propName && propValue) {
                    inlineCss << propName->getToken()->getText() << ": " 
                             << propValue->getToken()->getText() << "; ";
                }
            }
        }
    }
    
    info.globalCss = globalCss.str();
    info.inlineCss = inlineCss.str();
    
    return info;
}

std::string AutoClassIdProcessor::extractClassName(const std::string& selector) {
    if (!selector.empty() && selector[0] == '.') {
        return selector.substr(1);
    }
    return "";
}

std::string AutoClassIdProcessor::extractIdName(const std::string& selector) {
    if (!selector.empty() && selector[0] == '#') {
        return selector.substr(1);
    }
    return "";
}

std::string AutoClassIdProcessor::processContextSymbol(const std::string& selector,
                                                      const std::string& currentClass,
                                                      const std::string& currentId) {
    std::string result = selector;
    
    // 优先使用类名
    std::string replacement = currentClass.empty() ? 
                            (currentId.empty() ? "" : "#" + currentId) : 
                            "." + currentClass;
    
    // 替换所有的 & 符号
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, replacement);
        pos += replacement.length();
    }
    
    return result;
}

std::string AutoClassIdProcessor::mergeClasses(const std::string& existingClasses,
                                              const std::unordered_set<std::string>& newClasses) {
    std::unordered_set<std::string> allClasses;
    
    // 解析现有类名
    std::istringstream iss(existingClasses);
    std::string cls;
    while (iss >> cls) {
        if (!cls.empty()) {
            allClasses.insert(cls);
        }
    }
    
    // 添加新类名
    allClasses.insert(newClasses.begin(), newClasses.end());
    
    // 构建结果字符串
    std::string result;
    for (const auto& c : allClasses) {
        if (!result.empty()) result += " ";
        result += c;
    }
    
    return result;
}

std::string AutoClassIdProcessor::generateCssRule(const std::string& selector,
                                                 const std::vector<std::pair<std::string, std::string>>& properties) {
    std::stringstream css;
    css << selector << " {\n";
    
    for (const auto& [prop, value] : properties) {
        css << "    " << prop << ": " << value << ";\n";
    }
    
    css << "}";
    return css.str();
}

} // namespace chtl