#include <iostream>
#include <string>

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
    
    // TODO: 实际的编译逻辑
    std::cout << "Note: This is a simplified version. Full compilation is not yet implemented." << std::endl;
    std::cout << "The project demonstrates the architecture and design of CHTL." << std::endl;
    
    return 0;
}