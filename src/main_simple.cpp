#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include "chtl/CHTLContext.h"
#include "chtl/CHTLMainCompiler.h"
#include "chtl/error/CHTLException.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "=================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.chtl> [-o output.html]" << std::endl;
        std::cout << std::endl;
        std::cout << "CHTL is a C++ Hypertext Language that allows you to write HTML" << std::endl;
        std::cout << "with a more developer-friendly syntax." << std::endl;
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
    
    std::cout << "Input file: " << inputFile << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    std::cout << std::endl;
    
    // 实际的编译逻辑
    try {
        // 创建CHTL上下文
        auto context = std::make_shared<chtl::CHTLContext>();
        
        // 创建主编译器
        chtl::CHTLMainCompiler compiler(context);
        
        // 设置编译选项
        if (outputFile.ends_with(".min.html")) {
            compiler.setMinifyHTML(true);
            compiler.setMinifyCSS(true);
            compiler.setMinifyJS(true);
        }
        
        // 编译文件
        std::cout << "Compiling CHTL file..." << std::endl;
        std::string result = compiler.compileFile(inputFile);
        
        if (result.empty()) {
            std::cerr << "Error: Compilation failed" << std::endl;
            return 1;
        }
        
        // 写入输出文件
        std::ofstream outFile(outputFile);
        if (!outFile) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return 1;
        }
        
        outFile << result;
        outFile.close();
        
        std::cout << "Compilation successful!" << std::endl;
        std::cout << "Output written to: " << outputFile << std::endl;
        
        // 显示编译统计
        auto stats = compiler.getStatistics();
        std::cout << "\nCompilation Statistics:" << std::endl;
        std::cout << "  Elements: " << stats.elementCount << std::endl;
        std::cout << "  Styles: " << stats.styleCount << std::endl;
        std::cout << "  Scripts: " << stats.scriptCount << std::endl;
        std::cout << "  Templates used: " << stats.templateCount << std::endl;
        std::cout << "  Custom elements: " << stats.customCount << std::endl;
        
    } catch (const chtl::CHTLException& e) {
        std::cerr << "CHTL Error: " << e.what() << std::endl;
        if (!e.getFile().empty()) {
            std::cerr << "  File: " << e.getFile() << std::endl;
            std::cerr << "  Line: " << e.getLine() << std::endl;
        }
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}