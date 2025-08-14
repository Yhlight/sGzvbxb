#ifndef CHTL_CODE_GEN_VISITOR_H
#define CHTL_CODE_GEN_VISITOR_H

#include <string>
#include <memory>
#include <sstream>
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
    
    // 访问不同的节点类型
    void visitCompilationUnit(std::shared_ptr<ParseContext> ctx);
    void visitHtmlElement(std::shared_ptr<ParseContext> ctx);
    void visitTemplateDefinition(std::shared_ptr<ParseContext> ctx);
    void visitImportStatement(std::shared_ptr<ParseContext> ctx);
    void visitText(std::shared_ptr<ParseContext> ctx);
    void visitStyleProperty(std::shared_ptr<ParseContext> ctx);
    
    // 辅助方法
    std::string getNodeText(std::shared_ptr<ParseTree> node);
    void visitChildren(std::shared_ptr<ParseContext> ctx);
};

} // namespace parser
} // namespace chtl

#endif // CHTL_CODE_GEN_VISITOR_H