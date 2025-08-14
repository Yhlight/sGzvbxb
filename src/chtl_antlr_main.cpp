#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "antlr4-runtime.h"
#include "../generated/chtl/grammars/CHTLLexer.h"
#include "../generated/chtl/grammars/CHTLParser.h"
#include "../generated/chtl/grammars/CHTLBaseVisitor.h"
#include "../generated/css/grammars/CSSLexer.h"
#include "../generated/css/grammars/CSSParser.h"
#include "../generated/javascript/grammars/JavaScriptLexer.h"
#include "../generated/javascript/grammars/JavaScriptParser.h"

using namespace antlr4;

// CHTL到HTML的访问器
class CHTLToHTMLVisitor : public CHTLBaseVisitor {
private:
    std::stringstream html;
    std::stringstream css;
    std::stringstream js;
    int indentLevel = 0;
    
    std::string indent() {
        return std::string(indentLevel * 2, ' ');
    }
    
public:
    virtual antlrcpp::Any visitProgram(CHTLParser::ProgramContext *ctx) override {
        // 访问所有顶层语句
        for (auto* stmt : ctx->topLevelStatement()) {
            visit(stmt);
        }
        return nullptr;
    }
    
    virtual antlrcpp::Any visitHtmlElement(CHTLParser::HtmlElementContext *ctx) override {
        std::string tagName = ctx->elementName()->getText();
        html << indent() << "<" << tagName;
        
        // 处理属性
        if (ctx->elementContent()) {
            auto content = ctx->elementContent();
            
            // 检查是否有属性
            for (auto* attr : content->attributeList()) {
                html << " " << attr->attributeName()->getText();
                if (attr->attributeValue()) {
                    std::string value = attr->attributeValue()->getText();
                    // 移除引号
                    if (value.length() >= 2 && (value[0] == '"' || value[0] == '\'')) {
                        value = value.substr(1, value.length() - 2);
                    }
                    html << "=\"" << value << "\"";
                }
            }
        }
        
        html << ">\n";
        
        // 处理子元素
        indentLevel++;
        if (ctx->elementContent()) {
            for (auto* child : ctx->elementContent()->contentItem()) {
                visit(child);
            }
        }
        indentLevel--;
        
        html << indent() << "</" << tagName << ">\n";
        
        return nullptr;
    }
    
    virtual antlrcpp::Any visitTextNode(CHTLParser::TextNodeContext *ctx) override {
        if (ctx->stringLiteral()) {
            std::string text = ctx->stringLiteral()->getText();
            // 移除引号
            if (text.length() >= 2) {
                text = text.substr(1, text.length() - 2);
            }
            html << indent() << text << "\n";
        }
        return nullptr;
    }
    
    virtual antlrcpp::Any visitStyleBlock(CHTLParser::StyleBlockContext *ctx) override {
        css << "\n/* Style block from CHTL */\n";
        if (ctx->cssContent()) {
            css << ctx->cssContent()->getText() << "\n";
        }
        return nullptr;
    }
    
    virtual antlrcpp::Any visitScriptBlock(CHTLParser::ScriptBlockContext *ctx) override {
        js << "\n/* Script block from CHTL */\n";
        if (ctx->jsContent()) {
            js << ctx->jsContent()->getText() << "\n";
        }
        return nullptr;
    }
    
    std::string generateHTML() {
        std::stringstream output;
        output << "<!DOCTYPE html>\n";
        output << "<html>\n";
        output << "<head>\n";
        output << "  <meta charset=\"UTF-8\">\n";
        output << "  <title>CHTL Generated Page</title>\n";
        
        // 添加CSS
        if (css.str().length() > 0) {
            output << "  <style>\n";
            output << css.str();
            output << "  </style>\n";
        }
        
        output << "</head>\n";
        output << "<body>\n";
        output << html.str();
        
        // 添加JavaScript
        if (js.str().length() > 0) {
            output << "  <script>\n";
            output << js.str();
            output << "  </script>\n";
        }
        
        output << "</body>\n";
        output << "</html>\n";
        
        return output.str();
    }
};

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler (ANTLR Version) v1.0.0" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "Powered by ANTLR4 generated parsers" << std::endl;
    std::cout << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.chtl> [-o output.html]" << std::endl;
        std::cout << std::endl;
        std::cout << "CHTL (C++ Hypertext Language) combines the power of C++ with" << std::endl;
        std::cout << "the simplicity of HTML for modern web development." << std::endl;
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
        
        // 设置错误处理
        parser.removeErrorListeners();
        parser.addErrorListener(&ConsoleErrorListener::INSTANCE);
        
        // 解析
        std::cout << "Parsing CHTL syntax..." << std::endl;
        tree::ParseTree *tree = parser.program();
        
        if (parser.getNumberOfSyntaxErrors() > 0) {
            std::cerr << "Syntax errors found. Compilation failed." << std::endl;
            return 1;
        }
        
        std::cout << "Parse completed successfully. Generating HTML..." << std::endl;
        
        // 创建访问器并生成HTML
        CHTLToHTMLVisitor visitor;
        visitor.visit(tree);
        
        // 获取生成的HTML
        std::string htmlOutput = visitor.generateHTML();
        
        // 写入输出文件
        std::ofstream output(outputFile);
        if (!output.is_open()) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return 1;
        }
        
        output << htmlOutput;
        output.close();
        
        std::cout << "Output written to: " << outputFile << std::endl;
        std::cout << "Compilation completed successfully!" << std::endl;
        
        // 显示一些统计信息
        std::cout << std::endl;
        std::cout << "Statistics:" << std::endl;
        std::cout << "  Input size: " << source.length() << " bytes" << std::endl;
        std::cout << "  Output size: " << htmlOutput.length() << " bytes" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}