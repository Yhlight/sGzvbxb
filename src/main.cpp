// CHTL Compiler Main Entry Point
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "chtl/CHTLContext.h"
#include "chtl/scanner/CHTLUnifiedScanner.h"
#include "chtl/generator/HtmlGenerator.h"
#include "chtl/error/ErrorInterface.h"
#include "chtl/parser/CHTLSimpleParser.h"

using namespace chtl;

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "===================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.chtl> [-o output.html]" << std::endl;
        std::cout << std::endl;
        std::cout << "CHTL (C++ Hypertext Language) is a modern web development language" << std::endl;
        std::cout << "that combines the simplicity of HTML with the power of C++." << std::endl;
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
        
        std::cout << "Compiling: " << inputFile << std::endl;
        
        // 创建简单的CHTL解析器
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
        
        std::cout << "\nOutput written to: " << outputFile << std::endl;
        std::cout << "Compilation completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}