#include "ConfigLexer.h"
#include <cctype>
#include <stdexcept>

namespace chtl {
namespace parser {

ConfigLexer::ConfigLexer(const std::string& input) 
    : input_(input), position_(0), line_(1), column_(1) {}

std::vector<std::shared_ptr<Token>> ConfigLexer::tokenize() {
    std::vector<std::shared_ptr<Token>> tokens;
    
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        
        if (isAtEnd()) break;
        
        auto token = nextToken();
        if (token) {
            tokens.push_back(token);
        }
    }
    
    // 添加EOF标记
    tokens.push_back(std::make_shared<Token>(TokenType::EOF_TOKEN, "", line_, column_));
    
    return tokens;
}

std::shared_ptr<Token> ConfigLexer::nextToken() {
    int startLine = line_;
    int startColumn = column_;
    
    char c = advance();
    
    // 标识符或关键字
    if (std::isalpha(c) || c == '_' || c == '@') {
        return scanIdentifierOrKeyword(startLine, startColumn);
    }
    
    // 数字
    if (std::isdigit(c)) {
        return scanNumber(startLine, startColumn);
    }
    
    // 字符串
    if (c == '"' || c == '\'') {
        return scanString(c, startLine, startColumn);
    }
    
    // 操作符和分隔符
    switch (c) {
        case '[':
            return std::make_shared<Token>(TokenType::LBRACKET, "[", startLine, startColumn);
        case ']':
            return std::make_shared<Token>(TokenType::RBRACKET, "]", startLine, startColumn);
        case '{':
            return std::make_shared<Token>(TokenType::LBRACE, "{", startLine, startColumn);
        case '}':
            return std::make_shared<Token>(TokenType::RBRACE, "}", startLine, startColumn);
        case '(':
            return std::make_shared<Token>(TokenType::LPAREN, "(", startLine, startColumn);
        case ')':
            return std::make_shared<Token>(TokenType::RPAREN, ")", startLine, startColumn);
        case ',':
            return std::make_shared<Token>(TokenType::COMMA, ",", startLine, startColumn);
        case ';':
            return std::make_shared<Token>(TokenType::SEMICOLON, ";", startLine, startColumn);
        case ':':
            return std::make_shared<Token>(TokenType::COLON, ":", startLine, startColumn);
        case '=':
            return std::make_shared<Token>(TokenType::EQUALS, "=", startLine, startColumn);
        case '.':
            return std::make_shared<Token>(TokenType::DOT, ".", startLine, startColumn);
        case '-':
            if (peek() == '>') {
                advance();
                return std::make_shared<Token>(TokenType::ARROW, "->", startLine, startColumn);
            }
            return std::make_shared<Token>(TokenType::MINUS, "-", startLine, startColumn);
        case '+':
            return std::make_shared<Token>(TokenType::PLUS, "+", startLine, startColumn);
        case '*':
            return std::make_shared<Token>(TokenType::STAR, "*", startLine, startColumn);
        case '/':
            return std::make_shared<Token>(TokenType::SLASH, "/", startLine, startColumn);
        case '!':
            if (peek() == '=') {
                advance();
                return std::make_shared<Token>(TokenType::BANG_EQUAL, "!=", startLine, startColumn);
            }
            return std::make_shared<Token>(TokenType::BANG, "!", startLine, startColumn);
        case '<':
            if (peek() == '=') {
                advance();
                return std::make_shared<Token>(TokenType::LESS_EQUAL, "<=", startLine, startColumn);
            }
            return std::make_shared<Token>(TokenType::LESS, "<", startLine, startColumn);
        case '>':
            if (peek() == '=') {
                advance();
                return std::make_shared<Token>(TokenType::GREATER_EQUAL, ">=", startLine, startColumn);
            }
            return std::make_shared<Token>(TokenType::GREATER, ">", startLine, startColumn);
        default:
            throw std::runtime_error("Unexpected character: " + std::string(1, c));
    }
}

std::shared_ptr<Token> ConfigLexer::scanIdentifierOrKeyword(int startLine, int startColumn) {
    std::string text;
    
    // 处理@前缀
    if (current() == '@') {
        text += '@';
        advance();
    }
    
    // 扫描标识符
    while (std::isalnum(current()) || current() == '_') {
        text += current();
        advance();
    }
    
    // 检查是否是关键字
    TokenType type = TokenType::IDENTIFIER;
    
    if (text == "Configuration") type = TokenType::KEYWORD_CONFIGURATION;
    else if (text == "Name") type = TokenType::KEYWORD_NAME;
    else if (text == "Template") type = TokenType::KEYWORD_TEMPLATE;
    else if (text == "Custom") type = TokenType::KEYWORD_CUSTOM;
    else if (text == "Import") type = TokenType::KEYWORD_IMPORT;
    else if (text == "true" || text == "false") type = TokenType::BOOLEAN;
    else if (text == "null") type = TokenType::NULL_LITERAL;
    else if (text[0] == '@') {
        // @开头的特殊标识符
        if (text == "@Style") type = TokenType::KEYWORD_STYLE;
        else if (text == "@Element") type = TokenType::KEYWORD_ELEMENT;
        else if (text == "@Var") type = TokenType::KEYWORD_VAR;
    }
    
    return std::make_shared<Token>(type, text, startLine, startColumn);
}

std::shared_ptr<Token> ConfigLexer::scanNumber(int startLine, int startColumn) {
    std::string text;
    
    while (std::isdigit(current())) {
        text += current();
        advance();
    }
    
    // 处理小数
    if (current() == '.' && std::isdigit(peek())) {
        text += current();
        advance();
        
        while (std::isdigit(current())) {
            text += current();
            advance();
        }
    }
    
    return std::make_shared<Token>(TokenType::NUMBER, text, startLine, startColumn);
}

std::shared_ptr<Token> ConfigLexer::scanString(char quote, int startLine, int startColumn) {
    std::string text;
    
    // 跳过开始引号
    
    while (!isAtEnd() && current() != quote) {
        if (current() == '\\') {
            advance();
            if (!isAtEnd()) {
                // 简单的转义处理
                char escaped = current();
                switch (escaped) {
                    case 'n': text += '\n'; break;
                    case 't': text += '\t'; break;
                    case 'r': text += '\r'; break;
                    case '\\': text += '\\'; break;
                    case '"': text += '"'; break;
                    case '\'': text += '\''; break;
                    default: text += escaped; break;
                }
                advance();
            }
        } else {
            text += current();
            advance();
        }
    }
    
    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string");
    }
    
    // 跳过结束引号
    advance();
    
    return std::make_shared<Token>(TokenType::STRING_LITERAL, text, startLine, startColumn);
}

void ConfigLexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        if (std::isspace(current())) {
            advance();
        } else if (current() == '/' && peek() == '/') {
            // 单行注释
            advance(); // /
            advance(); // /
            while (!isAtEnd() && current() != '\n') {
                advance();
            }
        } else if (current() == '/' && peek() == '*') {
            // 多行注释
            advance(); // /
            advance(); // *
            while (!isAtEnd() && !(current() == '*' && peek() == '/')) {
                advance();
            }
            if (!isAtEnd()) {
                advance(); // *
                advance(); // /
            }
        } else {
            break;
        }
    }
}

bool ConfigLexer::isAtEnd() const {
    return position_ >= input_.length();
}

char ConfigLexer::current() const {
    if (isAtEnd()) return '\0';
    return input_[position_];
}

char ConfigLexer::peek() const {
    if (position_ + 1 >= input_.length()) return '\0';
    return input_[position_ + 1];
}

char ConfigLexer::advance() {
    if (isAtEnd()) return '\0';
    
    char c = input_[position_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

} // namespace parser
} // namespace chtl