#include "ConfigurationSystem.h"
#include "../parser/standalone/ConfigParser.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace chtl {
namespace config {

// 辅助函数：将ConfigValue转换为字符串
std::string configValueToString(const ConfigValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += vec[i];
        }
        result += "]";
        return result;
    }
    return "";
}

// 辅助函数：从字符串创建ConfigValue
ConfigValue parseConfigValue(const std::string& val) {
    // 尝试解析为不同类型
    if (val == "true" || val == "false") {
        return ConfigValue(val == "true");
    } else {
        // 尝试解析为整数
        try {
            size_t pos;
            int intVal = std::stoi(val, &pos);
            if (pos == val.length()) {
                return ConfigValue(intVal);
            } else {
                return ConfigValue(val); // 字符串
            }
        } catch (...) {
            return ConfigValue(val); // 字符串
        }
    }
}

// 单例实现
ConfigurationSystem& ConfigurationSystem::getInstance() {
    static ConfigurationSystem instance;
    return instance;
}

ConfigurationSystem::ConfigurationSystem() {
    // 设置默认值
    reset();
}

void ConfigurationSystem::reset() {
    configItems.clear();
    indexInitialCount = 0;
    disableNameGroup = true;
    debugMode = false;
    nameGroup = NameGroup();
}

bool ConfigurationSystem::loadConfigurationFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open configuration file: " << filePath << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return loadConfiguration(buffer.str());
}

bool ConfigurationSystem::loadConfiguration(const std::string& configContent) {
    // 使用独立的配置解析器（不依赖ANTLR）
    parser::ConfigLexer lexer(configContent);
    auto tokens = lexer.tokenize();
    auto tokenStream = std::make_shared<parser::TokenStream>(tokens);
    
    parser::ConfigParser parser(tokenStream);
    auto result = parser.parse();
    
    if (!result.success) {
        std::cerr << "Configuration parse error: " << result.error << std::endl;
        return false;
    }
    
    // 清空现有配置
    reset();
    
    // 处理解析结果
    for (const auto& [groupName, entries] : result.groups) {
        if (groupName == "Configuration" || groupName == "DEFAULT" || groupName.empty()) {
            // 配置组
            for (const auto& [key, value] : entries) {
                configItems[key] = parseConfigValue(value);
                
                // 更新特殊配置项
                if (key == "indexInitialCount") {
                    indexInitialCount = std::stoi(value);
                } else if (key == "disableNameGroup") {
                    disableNameGroup = (value == "true");
                } else if (key == "debugMode") {
                    debugMode = (value == "true");
                }
            }
        } else if (groupName == "Name") {
            // 名称组 - 创建配置项映射
            std::unordered_map<std::string, ConfigValue> nameItems;
            for (const auto& [key, value] : entries) {
                nameItems[key] = parseConfigValue(value);
            }
            nameGroup.initialize(nameItems);
        }
    }
    
    return validateConfiguration();
}

bool ConfigurationSystem::validateConfiguration() const {
    // 基本验证
    return true;
}

std::string ConfigurationSystem::exportConfiguration() const {
    std::stringstream ss;
    
    ss << "# CHTL Configuration\n\n";
    ss << "[Configuration]\n";
    
    for (const auto& [key, value] : configItems) {
        ss << key << " = " << configValueToString(value) << "\n";
    }
    
    // TODO: 导出NameGroup配置
    
    return ss.str();
}

bool ConfigurationSystem::isKeyword(const std::string& token, const std::string& keywordType) const {
    // 简单实现
    if (keywordType == "html") {
        return token == "html" || token == "body" || token == "div" || 
               token == "p" || token == "h1" || token == "h2" || token == "h3";
    }
    return false;
}

ConfigValue ConfigurationSystem::parseValue(const std::string& value) {
    return parseConfigValue(value);
}

std::vector<std::string> ConfigurationSystem::parseArrayValue(const std::string& value) {
    std::vector<std::string> result;
    // 简单的数组解析
    if (value.front() == '[' && value.back() == ']') {
        std::string content = value.substr(1, value.length() - 2);
        std::stringstream ss(content);
        std::string item;
        while (std::getline(ss, item, ',')) {
            // 去除空白
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            result.push_back(item);
        }
    }
    return result;
}

// 模板特化已经在头文件中内联定义

// NameGroup实现
NameGroup::NameGroup() {
    // 设置默认值
    customStyle = {"@Style", "@style", "@CSS", "@Css", "@css"};
    customElement = "@Element";
    customVar = "@Var";
    
    templateStyle = "@Style";
    templateElement = "@Element";
    templateVar = "@Var";
    
    originHtml = "@Html";
    originStyle = "@Style";
    originJavaScript = "@JavaScript";
    
    importHtml = "@Html";
    importStyle = "@Style";
    importJavaScript = "@JavaScript";
    importChtl = "@Chtl";
    importCJmod = "@CJmod";
    
    keywordInherit = "inherit";
    keywordDelete = "delete";
    keywordInsert = "insert";
    keywordAfter = "after";
    keywordBefore = "before";
    keywordReplace = "replace";
    keywordAtTop = "at top";
    keywordAtBottom = "at bottom";
    keywordFrom = "from";
    keywordAs = "as";
    keywordExcept = "except";
    
    keywordText = "text";
    keywordStyle = "style";
    keywordScript = "script";
    
    keywordCustom = "[Custom]";
    keywordTemplate = "[Template]";
    keywordOrigin = "[Origin]";
    keywordImport = "[Import]";
    keywordNamespace = "[Namespace]";
}

void NameGroup::initialize(const std::unordered_map<std::string, ConfigValue>& items) {
    // 辅助函数：获取字符串值
    auto getString = [&items](const std::string& key, const std::string& defaultValue) -> std::string {
        auto it = items.find(key);
        if (it != items.end() && std::holds_alternative<std::string>(it->second)) {
            return std::get<std::string>(it->second);
        }
        return defaultValue;
    };
    
    // 辅助函数：获取字符串数组值
    auto getStringArray = [&items](const std::string& key, const std::vector<std::string>& defaultValue) -> std::vector<std::string> {
        auto it = items.find(key);
        if (it != items.end() && std::holds_alternative<std::vector<std::string>>(it->second)) {
            return std::get<std::vector<std::string>>(it->second);
        }
        return defaultValue;
    };
    
    // 初始化各个字段
    customStyle = getStringArray("CUSTOM_STYLE", customStyle);
    customElement = getString("CUSTOM_ELEMENT", customElement);
    customVar = getString("CUSTOM_VAR", customVar);
    
    templateStyle = getString("TEMPLATE_STYLE", templateStyle);
    templateElement = getString("TEMPLATE_ELEMENT", templateElement);
    templateVar = getString("TEMPLATE_VAR", templateVar);
    
    originHtml = getString("ORIGIN_HTML", originHtml);
    originStyle = getString("ORIGIN_STYLE", originStyle);
    originJavaScript = getString("ORIGIN_JAVASCRIPT", originJavaScript);
    
    importHtml = getString("IMPORT_HTML", importHtml);
    importStyle = getString("IMPORT_STYLE", importStyle);
    importJavaScript = getString("IMPORT_JAVASCRIPT", importJavaScript);
    importChtl = getString("IMPORT_CHTL", importChtl);
    importCJmod = getString("IMPORT_CJMOD", importCJmod);
    
    keywordInherit = getString("KEYWORD_INHERIT", keywordInherit);
    keywordDelete = getString("KEYWORD_DELETE", keywordDelete);
    keywordInsert = getString("KEYWORD_INSERT", keywordInsert);
    keywordAfter = getString("KEYWORD_AFTER", keywordAfter);
    keywordBefore = getString("KEYWORD_BEFORE", keywordBefore);
    keywordReplace = getString("KEYWORD_REPLACE", keywordReplace);
    keywordAtTop = getString("KEYWORD_ATTOP", keywordAtTop);
    keywordAtBottom = getString("KEYWORD_ATBOTTOM", keywordAtBottom);
    keywordFrom = getString("KEYWORD_FROM", keywordFrom);
    keywordAs = getString("KEYWORD_AS", keywordAs);
    keywordExcept = getString("KEYWORD_EXCEPT", keywordExcept);
    
    keywordText = getString("KEYWORD_TEXT", keywordText);
    keywordStyle = getString("KEYWORD_STYLE", keywordStyle);
    keywordScript = getString("KEYWORD_SCRIPT", keywordScript);
    
    keywordCustom = getString("KEYWORD_CUSTOM", keywordCustom);
    keywordTemplate = getString("KEYWORD_TEMPLATE", keywordTemplate);
    keywordOrigin = getString("KEYWORD_ORIGIN", keywordOrigin);
    keywordImport = getString("KEYWORD_IMPORT", keywordImport);
    keywordNamespace = getString("KEYWORD_NAMESPACE", keywordNamespace);
    
    // 获取选项数量限制
    auto it = items.find("OPTION_COUNT");
    if (it != items.end() && std::holds_alternative<int>(it->second)) {
        optionCount = std::get<int>(it->second);
    }
}

bool NameGroup::validate() const {
    // 验证组选项数量
    if (customStyle.size() > static_cast<size_t>(optionCount)) {
        std::cerr << "Warning: CUSTOM_STYLE has " << customStyle.size() 
                  << " options, exceeding OPTION_COUNT limit of " << optionCount << std::endl;
    }
    
    // 验证必需字段不为空
    if (customElement.empty() || customVar.empty() ||
        templateStyle.empty() || templateElement.empty() || templateVar.empty()) {
        return false;
    }
    
    return true;
}

std::vector<std::string> NameGroup::getAllKeywordVariants(const std::string& keywordType) const {
    if (keywordType == "CUSTOM_STYLE") {
        return customStyle;
    } else if (keywordType == "CUSTOM_ELEMENT") {
        return {customElement};
    } else if (keywordType == "CUSTOM_VAR") {
        return {customVar};
    }
    // ... 其他类型类似处理
    
    return {};
}

} // namespace config
} // namespace chtl