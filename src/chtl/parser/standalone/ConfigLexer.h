#ifndef CHTL_STANDALONE_CONFIG_LEXER_H
#define CHTL_STANDALONE_CONFIG_LEXER_H

#include "ParserRuntime.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace parser {

class ConfigLexer {
public:
    explicit ConfigLexer(const std::string& input);
    
    // 词法分析，返回所有token
    std::vector<std::shared_ptr<Token>> tokenize();
    
private:
    std::string input_;
    size_t position_;
    int line_;
    int column_;
    
    // 扫描下一个token
    std::shared_ptr<Token> nextToken();
    
    // 扫描标识符或关键字
    std::shared_ptr<Token> scanIdentifierOrKeyword(int startLine, int startColumn);
    
    // 扫描数字
    std::shared_ptr<Token> scanNumber(int startLine, int startColumn);
    
    // 扫描字符串
    std::shared_ptr<Token> scanString(char quote, int startLine, int startColumn);
    
    // 跳过空白和注释
    void skipWhitespaceAndComments();
    
    // 辅助方法
    bool isAtEnd() const;
    char current() const;
    char peek() const;
    char advance();
};

} // namespace parser
} // namespace chtl

#endif // CHTL_STANDALONE_CONFIG_LEXER_H