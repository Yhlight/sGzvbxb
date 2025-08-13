// CHTL Compiler Main Entry Point
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "chtl/CHTLContext.h"
#include "chtl/scanner/CHTLUnifiedScanner.h"
#include "chtl/generator/HtmlGenerator.h"
#include "chtl/error/ErrorInterface.h"

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
        
        // 创建上下文
        auto context = std::make_shared<CHTLContext>();
        
        // 创建扫描器
        scanner::CHTLUnifiedScanner scanner;
        auto fragments = scanner.scan(source);
        
        if (scanner.hasErrors()) {
            std::cerr << "Scanner errors:" << std::endl;
            for (const auto& error : scanner.getErrors()) {
                std::cerr << "  " << error << std::endl;
            }
            return 1;
        }
        
        std::cout << "Scanned " << fragments.size() << " code fragments" << std::endl;
        
        // 创建HTML生成器
        generator::HtmlGenerator htmlGen(context);
        
        // 简单的演示：输出扫描到的片段
        std::cout << "\nCode fragments:" << std::endl;
        for (const auto& fragment : fragments) {
            std::cout << "  Type: " << static_cast<int>(fragment.type) 
                     << ", Lines: " << fragment.startLine << "-" << fragment.endLine
                     << ", Content length: " << fragment.content.length() << std::endl;
        }
        
        // 写入输出文件（目前只是简单的演示）
        std::ofstream output(outputFile);
        if (!output.is_open()) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return 1;
        }
        
        output << "<!DOCTYPE html>\n";
        output << "<html>\n";
        output << "<head>\n";
        output << "  <meta charset=\"UTF-8\">\n";
        output << "  <title>CHTL Generated Page</title>\n";
        output << "</head>\n";
        output << "<body>\n";
        output << "  <h1>CHTL Compilation Result</h1>\n";
        output << "  <p>Successfully processed " << fragments.size() << " fragments from " << inputFile << "</p>\n";
        output << "</body>\n";
        output << "</html>\n";
        
        output.close();
        
        std::cout << "\nOutput written to: " << outputFile << std::endl;
        std::cout << "Compilation completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}