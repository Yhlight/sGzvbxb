#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "antlr4-runtime.h"
#include "../generated/chtl/grammars/CHTLLexer.h"
#include "../generated/chtl/grammars/CHTLParser.h"
#include "../generated/chtl/grammars/CHTLBaseVisitor.h"

using namespace antlr4;

// 简单的CHTL访问器
class SimpleCHTLVisitor : public CHTLBaseVisitor {
public:
    std::stringstream html;
    
    virtual antlrcpp::Any visitProgram(CHTLParser::ProgramContext *ctx) override {
        html << "<!DOCTYPE html>\n<html>\n<head>\n";
        html << "  <meta charset=\"UTF-8\">\n";
        html << "  <title>CHTL Generated Page</title>\n";
        html << "</head>\n<body>\n";
        
        // 访问所有子节点
        visitChildren(ctx);
        
        html << "</body>\n</html>\n";
        return nullptr;
    }
    
    virtual antlrcpp::Any visitHtmlElement(CHTLParser::HtmlElementContext *ctx) override {
        // 获取元素名称
        std::string tagName = ctx->elementName() ? ctx->elementName()->getText() : "div";
        
        html << "<" << tagName;
        
        // 处理属性
        if (ctx->elementContent()) {
            // 访问元素内容
            visitChildren(ctx);
        }
        
        html << "</" << tagName << ">\n";
        
        return nullptr;
    }
    
    virtual antlrcpp::Any visitTextNode(CHTLParser::TextNodeContext *ctx) override {
        if (ctx->stringLiteral()) {
            std::string text = ctx->stringLiteral()->getText();
            // 移除引号
            if (text.length() >= 2) {
                text = text.substr(1, text.length() - 2);
            }
            html << text;
        }
        return nullptr;
    }
    
    std::string getHTML() const {
        return html.str();
    }
};

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler (ANTLR Version) v1.0.0" << std::endl;
    std::cout << "====================================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.chtl> [-o output.html]" << std::endl;
        return 0;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = "output.html";
    
    // 检查-o选项
    for (int i = 2; i < argc - 1; i++) {
        if (std::string(argv[i]) == "-o") {
            outputFile = argv[i + 1];
            break;
        }
    }
    
    try {
        // 读取输入文件
        std::ifstream input(inputFile);
        if (!input.is_open()) {
            std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
            return 1;
        }
        
        std::stringstream buffer;
        buffer << input.rdbuf();
        std::string source = buffer.str();
        input.close();
        
        std::cout << "Parsing: " << inputFile << std::endl;
        
        // 创建ANTLR输入流
        ANTLRInputStream antlrInput(source);
        
        // 创建词法分析器
        CHTLLexer lexer(&antlrInput);
        CommonTokenStream tokens(&lexer);
        
        // 创建语法分析器
        CHTLParser parser(&tokens);
        
        // 解析
        tree::ParseTree *tree = parser.program();
        
        std::cout << "Parse completed. Generating HTML..." << std::endl;
        
        // 创建访问器并生成HTML
        SimpleCHTLVisitor visitor;
        visitor.visit(tree);
        
        // 写入输出文件
        std::ofstream output(outputFile);
        if (!output.is_open()) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return 1;
        }
        
        output << visitor.getHTML();
        output.close();
        
        std::cout << "Output written to: " << outputFile << std::endl;
        std::cout << "Compilation completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}