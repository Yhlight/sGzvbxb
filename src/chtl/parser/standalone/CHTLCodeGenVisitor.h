#ifndef CHTL_CODE_GEN_VISITOR_H
#define CHTL_CODE_GEN_VISITOR_H

#include <string>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <set>
#include "ParserRuntime.h"

namespace chtl {
namespace parser {

// 代码生成访问器
class CHTLCodeGenVisitor {
public:
    CHTLCodeGenVisitor() {}
    
    // 访问解析树并生成 HTML
    std::string visit(std::shared_ptr<ParseContext> tree);
    
private:
    std::stringstream html_;
    std::stringstream css_;
    std::stringstream js_;
    
    // 模板存储
    struct StyleTemplate {
        std::unordered_map<std::string, std::string> properties;
        std::string baseTemplate; // 继承的基础模板
    };
    
    struct ElementTemplate {
        std::shared_ptr<ParseContext> content;
    };
    
    struct VarTemplate {
        std::unordered_map<std::string, std::string> variables;
    };
    
    std::unordered_map<std::string, StyleTemplate> styleTemplates_;
    std::unordered_map<std::string, ElementTemplate> elementTemplates_;
    std::unordered_map<std::string, VarTemplate> varTemplates_;
    
    // 自定义元素、样式和变量
    std::unordered_map<std::string, std::shared_ptr<ParseContext>> customElements_;
    std::unordered_map<std::string, std::string> customStyles_;
    std::unordered_map<std::string, std::string> customVars_;
    
    // 局部样式块的类名生成
    int localStyleCounter_ = 0;
    std::unordered_map<std::string, std::string> localStyleMapping_;
    
    // 命名的原始嵌入节点
    struct OriginNode {
        std::string type;
        std::string content;
    };
    std::unordered_map<std::string, OriginNode> namedOriginNodes_;
    
    // 访问不同的节点类型
    void visitCompilationUnit(std::shared_ptr<ParseContext> ctx);
    void visitHtmlElement(std::shared_ptr<ParseContext> ctx);
    void visitTemplateDefinition(std::shared_ptr<ParseContext> ctx);
    void visitCustomDefinition(std::shared_ptr<ParseContext> ctx);
    void visitImportStatement(std::shared_ptr<ParseContext> ctx);
    void visitText(std::shared_ptr<ParseContext> ctx);
    void visitStyleProperty(std::shared_ptr<ParseContext> ctx);
    void visitLocalStyleBlock(std::shared_ptr<ParseContext> ctx, const std::string& elementTag);
    void visitLocalScriptBlock(std::shared_ptr<ParseContext> ctx);
    void visitAttribute(std::shared_ptr<ParseContext> ctx);
    void visitOriginDeclaration(std::shared_ptr<ParseContext> ctx);
    void visitExceptConstraint(std::shared_ptr<ParseContext> ctx);
    
    // 模板处理
    void visitStyleTemplate(std::shared_ptr<ParseContext> ctx);
    void visitElementTemplate(std::shared_ptr<ParseContext> ctx);
    void visitVarTemplate(std::shared_ptr<ParseContext> ctx);
    void applyStyleTemplate(const std::string& templateName);
    void applyElementTemplate(const std::string& templateName);
    std::string resolveVariable(const std::string& varName);
    
    // CHTL JS 处理
    void generateCHTLJS();
    std::string generateDOMSelector(const std::string& selector);
    std::string generateChainAccess(const std::string& base, const std::string& member);
    std::string generateEventListener(const std::string& selector, const std::string& event, const std::string& handler);
    std::string generateAnimation(const std::string& selector, const std::string& animation);
    
    // 辅助方法
    std::string getNodeText(std::shared_ptr<ParseTree> node);
    void visitChildren(std::shared_ptr<ParseContext> ctx);
    std::string generateUniqueClassName();
    std::string transformCHTLJS(const std::string& code);
    void handleFileImport(std::shared_ptr<ParseContext> ctx);
    void handleSpecificImport(std::shared_ptr<ParseContext> ctx);
};

} // namespace parser
} // namespace chtl

#endif // CHTL_CODE_GEN_VISITOR_H