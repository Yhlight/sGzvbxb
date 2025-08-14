#include <iostream>
#include <string>
#include <cassert>
#include "../../src/chtl/CHTLMainCompiler.h"

using namespace chtl;

// 测试基本CHTL编译
void testBasicCHTL() {
    std::cout << "Test: Basic CHTL compilation..." << std::endl;
    
    CHTLMainCompiler compiler;
    
    std::string chtlCode = R"(
html {
    head {
        title { text { "Test Page" } }
    }
    body {
        h1 { text { "Hello CHTL" } }
        p { text { "This is a test." } }
    }
}
)";
    
    std::string result = compiler.compileString(chtlCode);
    
    // 验证输出包含预期的HTML元素
    assert(result.find("<html>") != std::string::npos);
    assert(result.find("<title>Test Page</title>") != std::string::npos);
    assert(result.find("<h1>Hello CHTL</h1>") != std::string::npos);
    
    std::cout << "✓ Basic CHTL test passed" << std::endl;
}

// 测试混合内容（CHTL + CSS + JS）
void testMixedContent() {
    std::cout << "Test: Mixed content compilation..." << std::endl;
    
    CHTLMainCompiler compiler;
    
    std::string mixedCode = R"(
html {
    head {
        style {
            body {
                background-color: #f0f0f0;
                font-family: Arial, sans-serif;
            }
            h1 {
                color: #333;
            }
        }
    }
    body {
        h1 { text { "Mixed Content Test" } }
        script {
            console.log("Hello from JavaScript!");
            function greet(name) {
                alert("Hello, " + name + "!");
            }
        }
    }
}
)";
    
    std::string result = compiler.compileString(mixedCode);
    
    // 验证各部分都被正确处理
    assert(result.find("<style>") != std::string::npos);
    assert(result.find("background-color") != std::string::npos);
    assert(result.find("<script>") != std::string::npos);
    assert(result.find("console.log") != std::string::npos);
    
    std::cout << "✓ Mixed content test passed" << std::endl;
}

// 测试CHTL JS特性
void testCHTLJS() {
    std::cout << "Test: CHTL JS compilation..." << std::endl;
    
    CHTLMainCompiler compiler;
    
    std::string chtljsCode = R"(
html {
    body {
        div #container {
            p { text { "Click me!" } }
        }
        script {
            // CHTL JS增强语法
            @Element container {
                on click {
                    this.style.backgroundColor = "yellow";
                }
            }
        }
    }
}
)";
    
    std::string result = compiler.compileString(chtljsCode);
    
    // 验证CHTL JS被处理
    assert(result.find("CHTL JS Enhanced Code") != std::string::npos);
    
    std::cout << "✓ CHTL JS test passed" << std::endl;
}

// 测试错误处理
void testErrorHandling() {
    std::cout << "Test: Error handling..." << std::endl;
    
    CHTLMainCompiler compiler;
    
    // 无效的CHTL代码
    std::string invalidCode = R"(
html {
    body {
        // 缺少闭合括号
        h1 { text { "Error Test"
    }
}
)";
    
    std::string result = compiler.compileString(invalidCode);
    auto errors = compiler.getErrors();
    
    // 应该有错误
    assert(!errors.empty());
    
    std::cout << "✓ Error handling test passed" << std::endl;
    std::cout << "  Detected " << errors.size() << " error(s)" << std::endl;
}

// 测试编译器隔离性
void testCompilerIsolation() {
    std::cout << "Test: Compiler isolation..." << std::endl;
    
    CHTLMainCompiler compiler;
    
    // 包含多种语言的代码
    std::string code = R"(
html {
    head {
        style {
            /* CSS注释 */
            .test { color: red; }
        }
    }
    body {
        div .test { text { "Test" } }
        script {
            // JavaScript注释
            var x = 10;
        }
    }
}
)";
    
    std::string result = compiler.compileString(code);
    
    // CSS和JS应该被各自的编译器处理
    assert(result.find("/* CSS注释 */") != std::string::npos || 
           result.find(".test") != std::string::npos);
    assert(result.find("// JavaScript注释") != std::string::npos || 
           result.find("var x") != std::string::npos);
    
    std::cout << "✓ Compiler isolation test passed" << std::endl;
}

int main() {
    std::cout << "=== CHTL New Architecture Tests ===" << std::endl;
    
    try {
        testBasicCHTL();
        testMixedContent();
        testCHTLJS();
        testErrorHandling();
        testCompilerIsolation();
        
        std::cout << "\n✅ All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}