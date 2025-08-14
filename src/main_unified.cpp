// CHTL Compiler Main Entry Point - Unified Architecture
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <chrono>
#include "chtl/scanner/CHTLUnifiedScanner.h"
#include "chtl/compiler/CompilerDispatcher.h"

using namespace chtl;

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " <input.chtl> [-o output.html] [--debug]\n";
    std::cerr << "Options:\n";
    std::cerr << "  -o <file>    Specify output file (default: output.html)\n";
    std::cerr << "  --debug      Enable debug output\n";
}

int main(int argc, char* argv[]) {
    std::cout << "\nCHTL Compiler v1.0.0 - Unified Architecture\n";
    std::cout << "============================================\n";
    
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // 解析命令行参数
    std::string inputFile = argv[1];
    std::string outputFile = "output.html";
    bool debugMode = false;
    
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (std::string(argv[i]) == "--debug") {
            debugMode = true;
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
        
        // 开始计时
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // 创建编译器调度器
        compiler::CompilerDispatcher dispatcher;
        dispatcher.setDebugMode(debugMode);
        
        // 编译源代码
        auto result = dispatcher.compile(source);
        
        // 结束计时
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        if (!result.success) {
            std::cerr << "\nCompilation failed:\n";
            for (const auto& error : result.errors) {
                std::cerr << "  Error: " << error << std::endl;
            }
            return 1;
        }
        
        // 写入输出文件
        std::ofstream output(outputFile);
        if (!output.is_open()) {
            std::cerr << "Error: Cannot open output file: " << outputFile << std::endl;
            return 1;
        }
        
        output << result.html;
        output.close();
        
        std::cout << "\n=== Compilation Statistics ===\n";
        std::cout << "Input file: " << inputFile << "\n";
        std::cout << "Output file: " << outputFile << "\n";
        std::cout << "Compilation time: " << duration.count() << " ms\n";
        std::cout << "Output size: " << result.html.length() << " bytes\n";
        
        if (debugMode) {
            std::cout << "\n=== Generated Resources ===\n";
            std::cout << "CSS size: " << result.css.length() << " bytes\n";
            std::cout << "JavaScript size: " << result.javascript.length() << " bytes\n";
        }
        
        std::cout << "\nCompilation completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}