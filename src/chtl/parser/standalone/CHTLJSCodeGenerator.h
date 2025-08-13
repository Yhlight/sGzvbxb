#ifndef CHTL_JS_CODE_GENERATOR_H
#define CHTL_JS_CODE_GENERATOR_H

#include <string>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "ParserRuntime.h"

namespace chtl {
namespace parser {

class CHTLJSCodeGenerator {
public:
    CHTLJSCodeGenerator();
    
    // 生成JavaScript代码
    std::string generate(std::shared_ptr<ParseContext> parseTree);
    
private:
    std::stringstream output_;
    int indentLevel_;
    std::unordered_map<std::string, std::string> selectorCache_;
    
    // 生成各种节点
    void generateNode(std::shared_ptr<ParseContext> node);
    void generateInterpolation(std::shared_ptr<ParseContext> node);
    void generateChainAccess(std::shared_ptr<ParseContext> node);
    void generateListen(std::shared_ptr<ParseContext> node);
    void generateAnimate(std::shared_ptr<ParseContext> node);
    void generateRawJS(std::shared_ptr<ParseContext> node);
    
    // 辅助方法
    void indent();
    void dedent();
    void writeIndent();
    void write(const std::string& text);
    void writeLine(const std::string& text);
    
    // 选择器转换
    std::string transformSelector(const std::string& selector);
    std::string generateSelectorFunction(const std::string& selector);
};

} // namespace parser
} // namespace chtl

#endif // CHTL_JS_CODE_GENERATOR_H