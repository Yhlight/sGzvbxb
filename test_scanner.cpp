#include <iostream>
#include <string>
#include "src/chtl/scanner/CHTLUnifiedScanner.h"

int main() {
    std::string simpleInput = "html { body { text { \"Hello\" } } }";
    
    std::cout << "Testing scanner with input:\n" << simpleInput << std::endl;
    
    chtl::scanner::CHTLUnifiedScanner scanner;
    
    std::cout << "Starting scan..." << std::endl;
    
    try {
        auto fragments = scanner.scan(simpleInput);
        
        std::cout << "Scan completed. Found " << fragments.size() << " fragments." << std::endl;
        
        for (size_t i = 0; i < fragments.size(); ++i) {
            const auto& fragment = fragments[i];
            std::cout << "Fragment " << i << ": Type=" << static_cast<int>(fragment.type) 
                      << ", Content length=" << fragment.content.length() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}