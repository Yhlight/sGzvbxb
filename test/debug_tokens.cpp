#include "../src/chtl/parser/standalone/CHTLLexer.h"
#include <iostream>
#include <fstream>

using namespace chtl::parser;

std::string tokenTypeToString(TokenType type) {
    switch(type) {
        case TokenType::KEYWORD_TEMPLATE: return "KEYWORD_TEMPLATE";
        case TokenType::KEYWORD_STYLE_GROUP: return "KEYWORD_STYLE_GROUP";
        case TokenType::AT: return "AT";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        case TokenType::KEYWORD_IMPORT: return "KEYWORD_IMPORT";
        case TokenType::KEYWORD_HTML: return "KEYWORD_HTML";
        case TokenType::KEYWORD_FROM: return "KEYWORD_FROM";
        case TokenType::KEYWORD_AS: return "KEYWORD_AS";
        default: return "UNKNOWN(" + std::to_string(static_cast<int>(type)) + ")";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <chtl_file>\n";
        return 1;
    }
    
    // 读取文件
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << argv[1] << "\n";
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // 词法分析
    CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    // 打印 tokens
    std::cout << "Tokens for file: " << argv[1] << "\n";
    std::cout << "==========================================\n";
    
    for (const auto& token : tokens) {
        std::cout << "Token: " << tokenTypeToString(token->getType()) 
                  << " | Text: \"" << token->getText() << "\""
                  << " | Line: " << token->getLine() 
                  << " | Column: " << token->getColumn() << "\n";
    }
    
    return 0;
}