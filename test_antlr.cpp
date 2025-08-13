#include <iostream>
#include <fstream>
#include <sstream>
#include "antlr4-runtime.h"
#include "generated/chtl/grammars/CHTLLexer.h"
#include "generated/chtl/grammars/CHTLParser.h"

using namespace antlr4;

int main(int argc, char* argv[]) {
    std::cout << "ANTLR CHTL Test" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.chtl>" << std::endl;
        return 0;
    }
    
    try {
        // 读取文件
        std::ifstream file(argv[1]);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // 创建ANTLR组件
        ANTLRInputStream input(content);
        CHTLLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        CHTLParser parser(&tokens);
        
        // 解析
        auto tree = parser.program();
        
        std::cout << "Parse successful!" << std::endl;
        std::cout << "Tree: " << tree->toStringTree(&parser) << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}