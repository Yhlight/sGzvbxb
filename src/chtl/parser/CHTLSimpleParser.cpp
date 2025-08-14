#include "CHTLSimpleParser.h"
#include <sstream>
#include <iostream>

namespace chtl {
namespace parser {

CHTLSimpleParser::CHTLSimpleParser() : pos_(0), line_(1), column_(1) {}

CHTLSimpleParser::ParseResult CHTLSimpleParser::parse(const std::string& input) {
    input_ = input;
    pos_ = 0;
    line_ = 1;
    column_ = 1;
    
    ParseResult result;
    result.success = true;
    
    try {
        skipWhitespace();
        result.root = parseElement();
        
        skipWhitespace();
        if (pos_ < input_.size()) {
            throw std::runtime_error("Unexpected content after root element");
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.error = e.what();
        result.errorLine = line_;
        result.errorColumn = column_;
    }
    
    return result;
}

std::shared_ptr<CHTLSimpleParser::Node> CHTLSimpleParser::parseElement() {
    auto node = std::make_shared<Node>();
    node->type = NodeType::ELEMENT;
    node->line = line_;
    node->column = column_;
    
    // 解析标签名
    node->tag = parseIdentifier();
    
    // 解析CSS选择器（类和ID）
    while (pos_ < input_.size() && (peek() == '.' || peek() == '#')) {
        if (peek() == '.') {
            advance(); // 消费 '.'
            std::string className = parseIdentifier();
            if (!className.empty()) {
                node->classes.push_back(className);
            }
        } else if (peek() == '#') {
            advance(); // 消费 '#'
            node->id = parseIdentifier();
        }
    }
    
    skipWhitespace();
    
    // 期待 {
    if (!consume('{')) {
        throw std::runtime_error("Expected '{' after element name");
    }
    
    // 解析属性和子元素
    while (true) {
        skipWhitespace();
        
        if (pos_ >= input_.size()) {
            throw std::runtime_error("Unexpected end of input in element");
        }
        
        // 检查是否结束
        if (peek() == '}') {
            advance();
            break;
        }
        
        // 保存当前位置以便回退
        size_t savePos = pos_;
        size_t saveLine = line_;
        size_t saveColumn = column_;
        
        // 尝试解析为属性（identifier: value;）
        try {
            std::string key = parseIdentifier();
            skipWhitespace();
            
            if (peek() == ':' || peek() == '=') {
                // 这是一个属性
                advance(); // 消费 ':' 或 '='
                skipWhitespace();
                
                std::string value;
                if (peek() == '"') {
                    value = parseStringLiteral();
                } else {
                    // 解析无引号的值直到分号
                    std::stringstream val;
                    while (pos_ < input_.size() && peek() != ';' && peek() != '}') {
                        val << peek();
                        advance();
                    }
                    value = val.str();
                    // 去除尾部空白
                    while (!value.empty() && std::isspace(value.back())) {
                        value.pop_back();
                    }
                }
                
                if (consume(';')) {
                    // 属性结束
                }
                
                node->attributes[key] = value;
                continue;
            }
        } catch (...) {
            // 不是属性，回退
        }
        
        // 回退到保存的位置
        pos_ = savePos;
        line_ = saveLine;
        column_ = saveColumn;
        
        // 检查是否是text块
        if (peekWord() == "text") {
            node->children.push_back(parseText());
        }
        // 检查是否是style块
        else if (peekWord() == "style") {
            node->children.push_back(parseStyle());
        }
        // 检查是否是script块
        else if (peekWord() == "script") {
            node->children.push_back(parseScript());
        }
        // 否则是子元素
        else {
            node->children.push_back(parseElement());
        }
    }
    
    return node;
}

std::shared_ptr<CHTLSimpleParser::Node> CHTLSimpleParser::parseText() {
    auto node = std::make_shared<Node>();
    node->type = NodeType::TEXT;
    node->line = line_;
    node->column = column_;
    
    // 消费 "text"
    parseIdentifier();
    
    skipWhitespace();
    
    // 期待 {
    if (!consume('{')) {
        throw std::runtime_error("Expected '{' after 'text'");
    }
    
    skipWhitespace();
    
    // 解析字符串字面量
    if (peek() == '"') {
        node->content = parseStringLiteral();
    } else {
        throw std::runtime_error("Expected string literal in text block");
    }
    
    skipWhitespace();
    
    // 期待 }
    if (!consume('}')) {
        throw std::runtime_error("Expected '}' after text content");
    }
    
    return node;
}

std::shared_ptr<CHTLSimpleParser::Node> CHTLSimpleParser::parseStyle() {
    auto node = std::make_shared<Node>();
    node->type = NodeType::STYLE;
    node->line = line_;
    node->column = column_;
    
    // 消费 "style"
    parseIdentifier();
    
    skipWhitespace();
    
    // 期待 {
    if (!consume('{')) {
        throw std::runtime_error("Expected '{' after 'style'");
    }
    
    // 收集style内容直到 }
    int braceDepth = 1;
    std::stringstream content;
    
    while (braceDepth > 0 && pos_ < input_.size()) {
        char c = peek();
        
        if (c == '{') {
            braceDepth++;
        } else if (c == '}') {
            braceDepth--;
            if (braceDepth == 0) {
                advance();
                break;
            }
        }
        
        content << c;
        advance();
    }
    
    node->content = content.str();
    return node;
}

std::shared_ptr<CHTLSimpleParser::Node> CHTLSimpleParser::parseScript() {
    auto node = std::make_shared<Node>();
    node->type = NodeType::SCRIPT;
    node->line = line_;
    node->column = column_;
    
    // 消费 "script"
    parseIdentifier();
    
    skipWhitespace();
    
    // 期待 {
    if (!consume('{')) {
        throw std::runtime_error("Expected '{' after 'script'");
    }
    
    // 收集script内容直到 }
    int braceDepth = 1;
    std::stringstream content;
    
    while (braceDepth > 0 && pos_ < input_.size()) {
        char c = peek();
        
        if (c == '{') {
            braceDepth++;
        } else if (c == '}') {
            braceDepth--;
            if (braceDepth == 0) {
                advance();
                break;
            }
        }
        
        content << c;
        advance();
    }
    
    node->content = content.str();
    return node;
}

std::string CHTLSimpleParser::parseIdentifier() {
    std::stringstream id;
    
    while (pos_ < input_.size() && (std::isalnum(peek()) || peek() == '_')) {
        id << peek();
        advance();
    }
    
    std::string result = id.str();
    if (result.empty()) {
        throw std::runtime_error("Expected identifier");
    }
    
    return result;
}

std::string CHTLSimpleParser::parseStringLiteral() {
    if (!consume('"')) {
        throw std::runtime_error("Expected '\"'");
    }
    
    std::stringstream str;
    
    while (pos_ < input_.size() && peek() != '"') {
        if (peek() == '\\') {
            advance();
            if (pos_ < input_.size()) {
                char c = peek();
                switch (c) {
                    case 'n': str << '\n'; break;
                    case 't': str << '\t'; break;
                    case 'r': str << '\r'; break;
                    case '\\': str << '\\'; break;
                    case '"': str << '"'; break;
                    default: str << c; break;
                }
                advance();
            }
        } else {
            str << peek();
            advance();
        }
    }
    
    if (!consume('"')) {
        throw std::runtime_error("Unterminated string literal");
    }
    
    return str.str();
}

void CHTLSimpleParser::skipWhitespace() {
    while (pos_ < input_.size() && std::isspace(peek())) {
        advance();
    }
}

char CHTLSimpleParser::peek() const {
    if (pos_ >= input_.size()) return '\0';
    return input_[pos_];
}

std::string CHTLSimpleParser::peekWord() const {
    size_t savePos = pos_;
    std::stringstream word;
    
    while (savePos < input_.size() && std::isalpha(input_[savePos])) {
        word << input_[savePos];
        savePos++;
    }
    
    return word.str();
}

void CHTLSimpleParser::advance() {
    if (pos_ < input_.size()) {
        if (input_[pos_] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        pos_++;
    }
}

bool CHTLSimpleParser::consume(char c) {
    if (peek() == c) {
        advance();
        return true;
    }
    return false;
}

// HTML生成器
std::string CHTLSimpleParser::generateHTML(std::shared_ptr<Node> node) {
    if (!node) return "";
    
    std::stringstream html;
    
    switch (node->type) {
        case NodeType::ELEMENT: {
            html << "<" << node->tag;
            
            // 输出ID
            if (!node->id.empty()) {
                html << " id=\"" << node->id << "\"";
            }
            
            // 输出类
            if (!node->classes.empty()) {
                html << " class=\"";
                for (size_t i = 0; i < node->classes.size(); ++i) {
                    if (i > 0) html << " ";
                    html << node->classes[i];
                }
                html << "\"";
            }
            
            // 输出其他属性
            for (const auto& [key, value] : node->attributes) {
                html << " " << key << "=\"" << value << "\"";
            }
            
            html << ">";
            
            // 输出子元素
            for (const auto& child : node->children) {
                html << generateHTML(child);
            }
            
            html << "</" << node->tag << ">";
            break;
        }
        
        case NodeType::TEXT:
            html << node->content;
            break;
            
        case NodeType::STYLE:
            html << "<style>" << node->content << "</style>";
            break;
            
        case NodeType::SCRIPT:
            html << "<script>" << node->content << "</script>";
            break;
    }
    
    return html.str();
}

} // namespace parser
} // namespace chtl