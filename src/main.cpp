// CHTL Compiler Main Entry Point
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "chtl/CHTLContext.h"
#include "chtl/CHTLMainCompiler.h"
#include "chtl/error/ErrorInterface.h"
#include "chtl/parser/CHTLSimpleParser.h"
#include "chtl/parser/standalone/CHTLParserEnhanced.h"
#include "chtl/parser/standalone/CHTLLexer.h"
#include "chtl/CHTLGenerator.h"

using namespace chtl;

int main(int argc, char* argv[]) {
    std::cout << "\nCHTL Compiler v1.0.0\n";
    std::cout << "===================\n";
    
    try {
        // 解析命令行参数
        std::string inputFile;
        std::string outputFile = "output.html";
        bool useFullCompiler = false;
        bool useEnhancedParser = false;
        
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <input.chtl> [-o output.html] [--full] [--enhanced]\n";
            std::cerr << "Options:\n";
            std::cerr << "  -o <file>    Specify output file (default: output.html)\n";
            std::cerr << "  --full       Use full compiler with advanced features\n";
            std::cerr << "  --enhanced   Use enhanced parser with complete CHTL syntax support\n";
            return 1;
        }
        
        inputFile = argv[1];
        
        // 解析其他参数
        for (int i = 2; i < argc; i++) {
            if (std::string(argv[i]) == "-o" && i + 1 < argc) {
                outputFile = argv[++i];
            } else if (std::string(argv[i]) == "--full") {
                useFullCompiler = true;
                std::cerr << "Note: Using full compiler mode with ANTLR support.\n";
            } else if (std::string(argv[i]) == "--enhanced") {
                useEnhancedParser = true;
                std::cerr << "Note: Using enhanced parser with complete CHTL syntax support.\n";
            }
        }
        
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
        
        std::cout << "Compiling: " << inputFile << std::endl;
        
        if (useFullCompiler) {
            std::cout << "Using full compiler with advanced features...\n";
            
            // 使用完整的CHTL编译器
            chtl::CHTLMainCompiler compiler;
            
            // 设置编译选项
            compiler.setStrictMode(false);
            compiler.setMinifyCSS(false);
            compiler.setMinifyJS(false);
            
            // 编译文件
            if (!compiler.compileFile(inputFile, outputFile)) {
                std::cerr << "Compilation failed:\n";
                
                // 输出错误
                const auto& errors = compiler.getErrors();
                for (const auto& error : errors) {
                    std::cerr << "  Error: " << error << std::endl;
                }
                
                // 输出警告
                const auto& warnings = compiler.getWarnings();
                for (const auto& warning : warnings) {
                    std::cerr << "  Warning: " << warning << std::endl;
                }
                
                return 1;
            }
            
        } else if (useEnhancedParser) {
            std::cout << "Using enhanced parser with complete CHTL syntax support...\n";
            
            // 使用增强解析器
            try {
                std::cout << "Creating lexer...\n";
                // 创建词法分析器
                parser::CHTLLexer lexer(source);
                std::cout << "Tokenizing source...\n";
                auto tokens = lexer.tokenize();
                std::cout << "Tokenization complete. Token count: " << tokens.size() << "\n";
                
                // 创建 TokenStream
                std::cout << "Creating TokenStream...\n";
                auto tokenStream = std::make_shared<parser::TokenStream>(tokens);
                
                // 创建增强解析器
                std::cout << "Creating enhanced parser...\n";
                parser::CHTLParserEnhanced parser(tokenStream);
                std::cout << "Parsing compilation unit...\n";
                auto parseTree = parser.compilationUnit();
                std::cout << "Parsing complete.\n";
                
                if (!parseTree) {
                    std::cerr << "Parse error: Failed to parse CHTL source\n";
                    return 1;
                }
                
                // 创建上下文和生成器
                auto generatorContext = std::make_shared<CHTLContext>();
                CHTLGenerator generator(generatorContext);
                generatorContext->setCurrentFile(inputFile);
                
                // TODO: 遍历解析树并生成代码
                // 这里暂时输出一个简单的成功消息
                std::cout << "Parsing successful!\n";
                std::cout << "Parse tree generated successfully.\n";
                
                // 生成基本输出
                std::ofstream output(outputFile);
                if (!output.is_open()) {
                    std::cerr << "Error: Cannot open output file: " << outputFile << std::endl;
                    return 1;
                }
                
                output << "<!DOCTYPE html>\n";
                output << "<html>\n";
                output << "<head>\n";
                output << "    <meta charset=\"UTF-8\">\n";
                output << "    <title>CHTL Enhanced Parser Output</title>\n";
                output << "</head>\n";
                output << "<body>\n";
                output << "    <h1>Enhanced Parser Test</h1>\n";
                output << "    <p>Successfully parsed CHTL source with enhanced parser.</p>\n";
                output << "</body>\n";
                output << "</html>\n";
                
                output.close();
                
            } catch (const std::exception& e) {
                std::cerr << "Parse error: " << e.what() << std::endl;
                return 1;
            }
            
        } else {
            std::cout << "Using simple parser (basic features only)...\n";
            
            // 使用简单解析器
            parser::CHTLSimpleParser parser;
            auto parseResult = parser.parse(source);
            
            if (!parseResult.success) {
                std::cerr << "Parse error at line " << parseResult.errorLine 
                         << ", column " << parseResult.errorColumn 
                         << ": " << parseResult.error << std::endl;
                return 1;
            }
            
            std::cout << "Parsing successful!" << std::endl;
            
            // 生成HTML
            std::string html = parser::CHTLSimpleParser::generateHTML(parseResult.root);
            
            // 写入输出文件
            std::ofstream output(outputFile);
            if (!output.is_open()) {
                std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
                return 1;
            }
            
            output << "<!DOCTYPE html>\n" << html;
            output.close();
        }
        
        std::cout << "\nOutput written to: " << outputFile << std::endl;
        std::cout << "Compilation completed successfully!" << std::endl;
        
    } catch (const chtl::error::CHTLException& e) {
        std::cerr << "\nCHTL Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}