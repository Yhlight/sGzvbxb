#include "../src/chtl/scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace chtl::scanner;

std::string fragmentTypeToString(FragmentType type) {
    switch(type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CHTL_JS: return "CHTL_JS";
        case FragmentType::CSS: return "CSS";
        case FragmentType::JAVASCRIPT: return "JAVASCRIPT";
        default: return "UNKNOWN";
    }
}

void printFragment(const CodeFragment& fragment, int index) {
    std::cout << "\n=== Fragment " << index << " [" << fragmentTypeToString(fragment.type) << "] ===\n";
    std::cout << "Position: " << fragment.startLine << ":" << fragment.startColumn 
              << " -> " << fragment.endLine << ":" << fragment.endColumn << "\n";
    std::cout << "Content:\n";
    std::cout << "---BEGIN---\n";
    std::cout << fragment.content;
    std::cout << "\n---END---\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.chtl>\n";
        return 1;
    }
    
    // 读取文件
    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Error: Cannot open file: " << argv[1] << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string source = buffer.str();
    input.close();
    
    std::cout << "Testing CHTLUnifiedScanner on: " << argv[1] << "\n";
    std::cout << "File size: " << source.length() << " bytes\n";
    
    // 创建扫描器
    CHTLUnifiedScanner scanner;
    scanner.setDebugMode(true);
    
    // 扫描源代码
    auto fragments = scanner.scan(source);
    
    std::cout << "\nTotal fragments: " << fragments.size() << "\n";
    
    // 打印每个片段
    int index = 1;
    for (const auto& fragment : fragments) {
        printFragment(fragment, index++);
    }
    
    // 统计
    int chtlCount = 0, chtlJsCount = 0, cssCount = 0, jsCount = 0;
    for (const auto& fragment : fragments) {
        switch(fragment.type) {
            case FragmentType::CHTL: chtlCount++; break;
            case FragmentType::CHTL_JS: chtlJsCount++; break;
            case FragmentType::CSS: cssCount++; break;
            case FragmentType::JAVASCRIPT: jsCount++; break;
        }
    }
    
    std::cout << "\n=== Statistics ===\n";
    std::cout << "CHTL fragments: " << chtlCount << "\n";
    std::cout << "CHTL JS fragments: " << chtlJsCount << "\n";
    std::cout << "CSS fragments: " << cssCount << "\n";
    std::cout << "JavaScript fragments: " << jsCount << "\n";
    
    return 0;
}