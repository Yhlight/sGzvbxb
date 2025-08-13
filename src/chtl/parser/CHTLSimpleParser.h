#ifndef CHTL_SIMPLE_PARSER_H
#define CHTL_SIMPLE_PARSER_H

#include <string>
#include <memory>
#include <vector>

namespace chtl {
namespace parser {

class CHTLSimpleParser {
public:
    enum class NodeType {
        ELEMENT,
        TEXT,
        STYLE,
        SCRIPT
    };
    
    struct Node {
        NodeType type;
        std::string tag;        // 对于ELEMENT节点
        std::string content;    // 对于TEXT, STYLE, SCRIPT节点
        std::vector<std::shared_ptr<Node>> children;
        int line;
        int column;
    };
    
    struct ParseResult {
        bool success;
        std::shared_ptr<Node> root;
        std::string error;
        int errorLine;
        int errorColumn;
    };
    
    CHTLSimpleParser();
    
    // 解析CHTL输入
    ParseResult parse(const std::string& input);
    
    // 生成HTML
    static std::string generateHTML(const std::shared_ptr<Node>& node);
    
private:
    std::string input_;
    size_t pos_;
    int line_;
    int column_;
    
    // 解析方法
    std::shared_ptr<Node> parseElement();
    std::shared_ptr<Node> parseText();
    std::shared_ptr<Node> parseStyle();
    std::shared_ptr<Node> parseScript();
    std::string parseIdentifier();
    std::string parseStringLiteral();
    
    // 辅助方法
    void skipWhitespace();
    char peek() const;
    std::string peekWord() const;
    void advance();
    bool consume(char c);
};

} // namespace parser
} // namespace chtl

#endif // CHTL_SIMPLE_PARSER_H