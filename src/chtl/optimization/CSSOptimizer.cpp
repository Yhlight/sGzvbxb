#include "PerformanceOptimizer.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace optimization {

CSSOptimizer::CSSOptimizer() {}

std::string CSSOptimizer::optimize(const std::string& css) {
    std::string result = css;
    
    // 1. 合并相同选择器
    result = mergeSelectors(result);
    
    // 2. 优化选择器性能
    result = optimizeSelectors(result);
    
    // 3. 压缩（如果需要）
    // result = minify(result);
    
    return result;
}

std::string CSSOptimizer::mergeSelectors(const std::string& css) {
    auto rules = parseCSS(css);
    
    // 按选择器分组
    std::unordered_map<std::string, std::vector<std::string>> selectorMap;
    
    for (const auto& rule : rules) {
        selectorMap[rule.selector].push_back(rule.properties);
    }
    
    // 重建CSS
    std::stringstream result;
    for (const auto& [selector, propsList] : selectorMap) {
        result << selector << " {\n";
        
        // 合并所有属性
        std::string merged = mergeProperties(propsList);
        result << merged;
        
        result << "}\n\n";
    }
    
    return result.str();
}

std::string CSSOptimizer::minify(const std::string& css) {
    std::string result;
    result.reserve(css.size());
    
    bool inComment = false;
    bool inString = false;
    char stringChar = 0;
    bool lastWasSpace = false;
    
    for (size_t i = 0; i < css.size(); ++i) {
        char c = css[i];
        
        // 处理注释
        if (!inString && !inComment && i + 1 < css.size() && 
            c == '/' && css[i + 1] == '*') {
            inComment = true;
            i++; // 跳过*
            continue;
        }
        
        if (inComment && i + 1 < css.size() && 
            c == '*' && css[i + 1] == '/') {
            inComment = false;
            i++; // 跳过/
            continue;
        }
        
        if (inComment) continue;
        
        // 处理字符串
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
            result += c;
            continue;
        }
        
        if (inString && c == stringChar && css[i - 1] != '\\') {
            inString = false;
            result += c;
            continue;
        }
        
        if (inString) {
            result += c;
            continue;
        }
        
        // 压缩空白
        if (std::isspace(c)) {
            if (!lastWasSpace && !result.empty() && 
                result.back() != '{' && result.back() != '}' &&
                result.back() != ';' && result.back() != ':') {
                result += ' ';
                lastWasSpace = true;
            }
            continue;
        }
        
        // 删除分号前的空格
        if (c == ';' && !result.empty() && result.back() == ' ') {
            result.pop_back();
        }
        
        // 删除冒号后的空格
        if (c == ':' && i + 1 < css.size() && css[i + 1] == ' ') {
            result += c;
            i++; // 跳过空格
            lastWasSpace = false;
            continue;
        }
        
        result += c;
        lastWasSpace = false;
    }
    
    return result;
}

std::string CSSOptimizer::optimizeSelectors(const std::string& css) {
    std::string result = css;
    
    // 优化通用选择器
    std::regex universalSelector(R"(\*\s*([^{]*)\{)");
    result = std::regex_replace(result, universalSelector, "$1{");
    
    // 优化后代选择器（将 > 用于直接子元素会更快）
    // 这需要更复杂的分析，暂时跳过
    
    // 优化属性选择器
    // 将慢速属性选择器转换为类选择器（需要配合HTML修改）
    
    return result;
}

std::vector<CSSOptimizer::CSSRule> CSSOptimizer::parseCSS(const std::string& css) {
    std::vector<CSSRule> rules;
    
    std::regex ruleRegex(R"(([^{]+)\{([^}]+)\})");
    std::sregex_iterator it(css.begin(), css.end(), ruleRegex);
    std::sregex_iterator end;
    
    while (it != end) {
        CSSRule rule;
        rule.selector = (*it)[1];
        rule.properties = (*it)[2];
        
        // 清理选择器
        rule.selector.erase(0, rule.selector.find_first_not_of(" \t\n\r"));
        rule.selector.erase(rule.selector.find_last_not_of(" \t\n\r") + 1);
        
        rule.specificity = calculateSpecificity(rule.selector);
        rules.push_back(rule);
        
        ++it;
    }
    
    return rules;
}

int CSSOptimizer::calculateSpecificity(const std::string& selector) {
    int a = 0, b = 0, c = 0, d = 0;
    
    // 简化的特异性计算
    // a: style属性（这里不适用）
    // b: ID选择器数量
    // c: 类、属性和伪类选择器数量
    // d: 元素和伪元素选择器数量
    
    // 计算ID选择器 (#id)
    std::regex idRegex(R"(#\w+)");
    auto idBegin = std::sregex_iterator(selector.begin(), selector.end(), idRegex);
    auto idEnd = std::sregex_iterator();
    b = std::distance(idBegin, idEnd);
    
    // 计算类选择器 (.class)
    std::regex classRegex(R"(\.\w+)");
    auto classBegin = std::sregex_iterator(selector.begin(), selector.end(), classRegex);
    auto classEnd = std::sregex_iterator();
    c = std::distance(classBegin, classEnd);
    
    // 计算元素选择器
    std::regex elementRegex(R"(\b[a-zA-Z]+\b)");
    auto elemBegin = std::sregex_iterator(selector.begin(), selector.end(), elementRegex);
    auto elemEnd = std::sregex_iterator();
    d = std::distance(elemBegin, elemEnd);
    
    // 返回组合的特异性值
    return a * 1000 + b * 100 + c * 10 + d;
}

std::string CSSOptimizer::mergeProperties(const std::vector<std::string>& props) {
    std::unordered_map<std::string, std::string> propertyMap;
    
    // 解析所有属性
    for (const auto& propBlock : props) {
        std::istringstream stream(propBlock);
        std::string line;
        
        while (std::getline(stream, line, ';')) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // 清理空白
                key.erase(0, key.find_first_not_of(" \t\n\r"));
                key.erase(key.find_last_not_of(" \t\n\r") + 1);
                value.erase(0, value.find_first_not_of(" \t\n\r"));
                value.erase(value.find_last_not_of(" \t\n\r") + 1);
                
                if (!key.empty() && !value.empty()) {
                    propertyMap[key] = value;  // 后面的覆盖前面的
                }
            }
        }
    }
    
    // 重建属性字符串
    std::stringstream result;
    for (const auto& [key, value] : propertyMap) {
        result << "    " << key << ": " << value << ";\n";
    }
    
    return result.str();
}

std::string CSSOptimizer::removeUnusedRules(const std::string& css,
                                           const std::set<std::string>& usedSelectors) {
    auto rules = parseCSS(css);
    std::stringstream result;
    
    for (const auto& rule : rules) {
        // 检查选择器是否被使用
        bool isUsed = false;
        
        // 简单匹配，实际应该更复杂
        for (const auto& used : usedSelectors) {
            if (rule.selector.find(used) != std::string::npos) {
                isUsed = true;
                break;
            }
        }
        
        if (isUsed) {
            result << rule.selector << " {\n" << rule.properties << "}\n\n";
        }
    }
    
    return result.str();
}

} // namespace optimization
} // namespace chtl