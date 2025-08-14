#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "chtl/parser/CHTLSimpleParser.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler Test v1.0.0" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // 检查参数
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.chtl> [output.html]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = (argc >= 3) ? argv[2] : "output.html";
    
    try {
        // 读取输入文件
        std::ifstream inFile(inputFile);
        if (!inFile) {
            std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
            return 1;
        }
        
        std::string input((std::istreambuf_iterator<char>(inFile)),
                         std::istreambuf_iterator<char>());
        inFile.close();
        
        // 使用简单解析器
        chtl::parser::CHTLSimpleParser parser;
        auto parseResult = parser.parse(input);
        
        if (!parseResult.success) {
            std::cerr << "Error: Failed to parse CHTL file" << std::endl;
            std::cerr << "Error message: " << parseResult.error << std::endl;
            return 1;
        }
        
        // 生成HTML
        std::string html = chtl::parser::CHTLSimpleParser::generateHTML(parseResult.root);
        
        // 写入输出文件
        std::ofstream outFile(outputFile);
        if (!outFile) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return 1;
        }
        
        outFile << html;
        outFile.close();
        
        std::cout << "Success! Output written to: " << outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}