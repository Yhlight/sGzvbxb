#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// 简单的CHTL演示编译器
// 展示了项目的架构和设计理念

class SimpleCHTLCompiler {
private:
    std::string source;
    std::stringstream html;
    std::stringstream css;
    std::stringstream js;
    
public:
    bool loadFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        source = buffer.str();
        return true;
    }
    
    void compile() {
        std::cout << "Compiling CHTL source..." << std::endl;
        
        // 简单的示例：识别基本的CHTL语法
        std::istringstream stream(source);
        std::string line;
        
        while (std::getline(stream, line)) {
            // 去除前导空格
            size_t start = line.find_first_not_of(" \t");
            if (start == std::string::npos) continue;
            
            line = line.substr(start);
            
            // 识别HTML元素
            if (line.find("html") == 0 || line.find("body") == 0 || 
                line.find("div") == 0 || line.find("h1") == 0 || 
                line.find("p") == 0) {
                
                size_t tagEnd = line.find_first_of(" {");
                std::string tag = line.substr(0, tagEnd);
                html << "<" << tag << ">\n";
            }
            // 识别text节点
            else if (line.find("text") == 0) {
                size_t start = line.find('"');
                size_t end = line.rfind('"');
                if (start != std::string::npos && end != std::string::npos && start < end) {
                    std::string text = line.substr(start + 1, end - start - 1);
                    html << text << "\n";
                }
            }
            // 识别style块
            else if (line.find("style") == 0) {
                css << "/* CHTL style block */\n";
            }
            // 识别script块
            else if (line.find("script") == 0) {
                js << "/* CHTL script block */\n";
            }
        }
        
        std::cout << "Compilation completed." << std::endl;
    }
    
    std::string generateHTML() {
        std::stringstream output;
        output << "<!DOCTYPE html>\n";
        output << "<html>\n";
        output << "<head>\n";
        output << "  <meta charset=\"UTF-8\">\n";
        output << "  <title>CHTL Demo Page</title>\n";
        
        if (!css.str().empty()) {
            output << "  <style>\n";
            output << css.str();
            output << "  </style>\n";
        }
        
        output << "</head>\n";
        output << html.str();
        
        if (!js.str().empty()) {
            output << "  <script>\n";
            output << js.str();
            output << "  </script>\n";
        }
        
        output << "</html>\n";
        
        return output.str();
    }
};

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "CHTL Compiler Demo v1.0.0" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "This demonstrates the CHTL architecture" << std::endl;
    std::cout << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.chtl> [-o output.html]" << std::endl;
        std::cout << std::endl;
        std::cout << "Example CHTL syntax:" << std::endl;
        std::cout << "  html {" << std::endl;
        std::cout << "    body {" << std::endl;
        std::cout << "      h1 {" << std::endl;
        std::cout << "        text { \"Hello CHTL!\" }" << std::endl;
        std::cout << "      }" << std::endl;
        std::cout << "    }" << std::endl;
        std::cout << "  }" << std::endl;
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
    
    SimpleCHTLCompiler compiler;
    
    std::cout << "Input: " << inputFile << std::endl;
    std::cout << "Output: " << outputFile << std::endl;
    std::cout << std::endl;
    
    if (!compiler.loadFile(inputFile)) {
        std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
        return 1;
    }
    
    compiler.compile();
    
    std::string html = compiler.generateHTML();
    
    std::ofstream output(outputFile);
    if (!output.is_open()) {
        std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
        return 1;
    }
    
    output << html;
    output.close();
    
    std::cout << std::endl;
    std::cout << "Output written to: " << outputFile << std::endl;
    std::cout << "File size: " << html.length() << " bytes" << std::endl;
    
    return 0;
}