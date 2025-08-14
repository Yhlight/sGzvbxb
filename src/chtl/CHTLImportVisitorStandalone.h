#pragma once

#include "parser/standalone/ParserRuntime.h"
#include "CHTLContext.h"
#include "CHTLTemplate.h"
#include "CHTLCustom.h"
#include "CHTLImport.h"
#include <memory>
#include <unordered_map>

namespace chtl {

/**
 * CHTL导入访问器（独立解析器版本）
 * 用于从独立解析器的解析树中提取模板和自定义定义
 */
class CHTLImportVisitorStandalone {
public:
    CHTLImportVisitorStandalone(std::shared_ptr<CHTLContext> context, 
                               const std::unordered_map<std::string, std::string>& nameMapping = {})
        : context_(context), nameMapping_(nameMapping) {}
    
    // 访问解析树
    void visit(std::shared_ptr<parser::ParseContext> tree);
    
    // 获取提取的模板
    const std::unordered_map<std::string, std::shared_ptr<Template>>& getTemplates() const {
        return templates;
    }
    
    // 获取提取的自定义定义
    const std::unordered_map<std::string, std::shared_ptr<CustomStyleGroup>>& getCustomStyles() const {
        return customStyles;
    }
    
    const std::unordered_map<std::string, std::shared_ptr<CustomElement>>& getCustomElements() const {
        return customElements;
    }
    
    const std::unordered_map<std::string, std::shared_ptr<CustomVarGroup>>& getCustomVars() const {
        return customVars;
    }
    
private:
    std::shared_ptr<CHTLContext> context_;
    std::unordered_map<std::string, std::string> nameMapping_;
    
    // 存储提取的定义
    std::unordered_map<std::string, std::shared_ptr<Template>> templates;
    std::unordered_map<std::string, std::shared_ptr<CustomStyleGroup>> customStyles;
    std::unordered_map<std::string, std::shared_ptr<CustomElement>> customElements;
    std::unordered_map<std::string, std::shared_ptr<CustomVarGroup>> customVars;
    
    // 访问特定节点
    void visitTemplateDefinition(std::shared_ptr<parser::ParseContext> ctx);
    void visitCustomDefinition(std::shared_ptr<parser::ParseContext> ctx);
    void visitChildren(std::shared_ptr<parser::ParseContext> ctx);
    
    // 辅助方法
    std::string getMappedName(const std::string& originalName) const;
    void parseStyleProperties(std::shared_ptr<parser::ParseContext> ctx, 
                            std::shared_ptr<StyleTemplate> styleTemplate);
    void parseElementContent(std::shared_ptr<parser::ParseContext> ctx,
                           std::shared_ptr<ElementTemplate> elementTemplate);
    void parseVarContent(std::shared_ptr<parser::ParseContext> ctx,
                        std::shared_ptr<VarTemplate> varTemplate);
    
    // 解析自定义内容
    void parseCustomStyleContent(std::shared_ptr<parser::ParseContext> ctx,
                                std::shared_ptr<CustomStyleGroup> customStyle);
    void parseCustomElementContent(std::shared_ptr<parser::ParseContext> ctx,
                                  std::shared_ptr<CustomElement> customElement);
    void parseCustomVarContent(std::shared_ptr<parser::ParseContext> ctx,
                              std::shared_ptr<CustomVarGroup> customVar);
};

/**
 * CHTL选择性导入访问器（独立解析器版本）
 * 仅提取指定名称的定义
 */
class CHTLSelectiveImportVisitorStandalone : public CHTLImportVisitorStandalone {
public:
    CHTLSelectiveImportVisitorStandalone(
        std::shared_ptr<CHTLContext> context,
        const std::vector<std::string>& templateNames,
        const std::vector<std::string>& customNames,
        const std::unordered_map<std::string, std::string>& nameMapping = {})
        : CHTLImportVisitorStandalone(context, nameMapping),
          targetTemplateNames_(templateNames.begin(), templateNames.end()),
          targetCustomNames_(customNames.begin(), customNames.end()) {}
          
private:
    std::unordered_set<std::string> targetTemplateNames_;
    std::unordered_set<std::string> targetCustomNames_;
    
    bool shouldImportTemplate(const std::string& name) const {
        return targetTemplateNames_.count(name) > 0;
    }
    
    bool shouldImportCustom(const std::string& name) const {
        return targetCustomNames_.count(name) > 0;
    }
};

} // namespace chtl