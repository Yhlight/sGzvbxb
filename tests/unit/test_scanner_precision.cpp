#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../../src/chtl/scanner/CHTLUnifiedScanner.h"

using namespace chtl::scanner;

void testCHTLJSDetection() {
    std::cout << "Test: CHTL JS detection..." << std::endl;
    
    CHTLUnifiedScanner scanner;
    
    // 测试1：包含CHTL JS特征的代码
    std::string code1 = R"(
script {
    {{button}}->addEventListener('click', () => {
        console.log("Clicked!");
    });
}
)";
    
    auto fragments1 = scanner.scan(code1);
    assert(fragments1.size() >= 1);
    bool hasCHTLJS = false;
    for (const auto& frag : fragments1) {
        if (frag.type == FragmentType::CHTL_JS) {
            hasCHTLJS = true;
            break;
        }
    }
    assert(hasCHTLJS);
    std::cout << "  ✓ Detected CHTL JS with -> operator" << std::endl;
    
    // 测试2：不包含CHTL JS特征的代码
    std::string code2 = R"(
script {
    var x = 10;
    console.log(x);
}
)";
    
    auto fragments2 = scanner.scan(code2);
    bool hasPlainJS = false;
    for (const auto& frag : fragments2) {
        if (frag.type == FragmentType::JAVASCRIPT) {
            hasPlainJS = true;
            break;
        }
    }
    assert(hasPlainJS);
    std::cout << "  ✓ Detected plain JS without CHTL JS features" << std::endl;
}

void testMixedScriptContent() {
    std::cout << "Test: Mixed script content with @Var..." << std::endl;
    
    CHTLUnifiedScanner scanner;
    
    std::string code = R"(
script {
    var bgColor = @Var(ThemeColor.background);
    document.body.style.backgroundColor = bgColor;
}
)";
    
    auto fragments = scanner.scan(code);
    
    // 应该有多个片段：JS + CHTL + JS
    int jsCount = 0;
    int chtlCount = 0;
    
    for (const auto& frag : fragments) {
        if (frag.type == FragmentType::JAVASCRIPT) jsCount++;
        if (frag.type == FragmentType::CHTL) chtlCount++;
        
        // 调试输出
        std::cout << "  Fragment type: " << static_cast<int>(frag.type) 
                  << ", content: " << frag.content.substr(0, 30) << "..." << std::endl;
    }
    
    assert(chtlCount > 0); // 应该识别出@Var
    std::cout << "  ✓ Correctly split @Var from JS code" << std::endl;
}

void testListenDetection() {
    std::cout << "Test: listen method detection..." << std::endl;
    
    CHTLUnifiedScanner scanner;
    
    std::string code = R"(
script {
    {{button}}.listen({
        click: function() { alert('Clicked!'); },
        hover: function() { this.style.color = 'red'; }
    });
}
)";
    
    auto fragments = scanner.scan(code);
    
    bool hasCHTLJS = false;
    for (const auto& frag : fragments) {
        if (frag.type == FragmentType::CHTL_JS) {
            hasCHTLJS = true;
            break;
        }
    }
    
    assert(hasCHTLJS);
    std::cout << "  ✓ Detected CHTL JS with listen method" << std::endl;
}

void testAnimateDetection() {
    std::cout << "Test: animate method detection..." << std::endl;
    
    CHTLUnifiedScanner scanner;
    
    std::string code = R"(
script {
    animate({
        element: {{box}},
        duration: 1000,
        begin: { opacity: 0 },
        end: { opacity: 1 }
    });
}
)";
    
    auto fragments = scanner.scan(code);
    
    bool hasCHTLJS = false;
    for (const auto& frag : fragments) {
        if (frag.type == FragmentType::CHTL_JS) {
            hasCHTLJS = true;
            break;
        }
    }
    
    assert(hasCHTLJS);
    std::cout << "  ✓ Detected CHTL JS with animate function" << std::endl;
}

void testStyleBlockDetection() {
    std::cout << "Test: Style block detection..." << std::endl;
    
    CHTLUnifiedScanner scanner;
    
    // 测试全局style
    std::string globalStyle = R"(
style {
    body { background: white; }
}
)";
    
    auto fragments1 = scanner.scan(globalStyle);
    bool hasCSS = false;
    for (const auto& frag : fragments1) {
        if (frag.type == FragmentType::CSS) {
            hasCSS = true;
            break;
        }
    }
    assert(hasCSS);
    std::cout << "  ✓ Global style detected as CSS" << std::endl;
    
    // 测试局部style
    std::string localStyle = R"(
div {
    style {
        color: red;
        .active { background: yellow; }
    }
}
)";
    
    auto fragments2 = scanner.scan(localStyle);
    bool hasCHTL = false;
    for (const auto& frag : fragments2) {
        if (frag.type == FragmentType::CHTL) {
            hasCHTL = true;
            break;
        }
    }
    assert(hasCHTL);
    std::cout << "  ✓ Local style detected as CHTL" << std::endl;
}

int main() {
    std::cout << "=== Scanner Precision Tests ===" << std::endl;
    
    try {
        testCHTLJSDetection();
        testMixedScriptContent();
        testListenDetection();
        testAnimateDetection();
        testStyleBlockDetection();
        
        std::cout << "\n✅ All scanner tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Scanner test failed: " << e.what() << std::endl;
        return 1;
    }
}