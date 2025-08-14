#include <iostream>
#include <cassert>
#include <string>
#include "../../src/chtl/compiler/handwritten/CHTLJSCompilerWrapper.h"
#include "../../src/chtl/scanner/CHTLUnifiedScanner.h"

using namespace chtl;

void testSelectorTransform() {
    std::cout << "Test: Selector transformation..." << std::endl;
    
    compiler::handwritten::CHTLJSCompilerWrapper compiler;
    scanner::CodeFragment fragment;
    fragment.type = scanner::FragmentType::CHTL_JS;
    
    // 测试1：简单选择器
    fragment.content = "{{button}}.textContent = 'Click me';";
    auto result1 = compiler.compile(fragment);
    assert(result1.success);
    assert(result1.output.find("_chtl.select('button')") != std::string::npos);
    std::cout << "  ✓ Simple selector transform" << std::endl;
    
    // 测试2：ID选择器
    fragment.content = "{{#myId}}.style.color = 'red';";
    auto result2 = compiler.compile(fragment);
    assert(result2.success);
    assert(result2.output.find("_chtl.select('#myId')") != std::string::npos);
    std::cout << "  ✓ ID selector transform" << std::endl;
    
    // 测试3：类选择器
    fragment.content = "{{.myClass}}.forEach(el => el.remove());";
    auto result3 = compiler.compile(fragment);
    assert(result3.success);
    assert(result3.output.find("_chtl.select('.myClass')") != std::string::npos);
    std::cout << "  ✓ Class selector transform" << std::endl;
    
    // 测试4：数组访问
    fragment.content = "{{button[0]}}.focus();";
    auto result4 = compiler.compile(fragment);
    assert(result4.success);
    assert(result4.output.find("_chtl.select('button')[0]") != std::string::npos);
    std::cout << "  ✓ Array access transform" << std::endl;
}

void testChainAccess() {
    std::cout << "Test: Chain access transformation..." << std::endl;
    
    compiler::handwritten::CHTLJSCompilerWrapper compiler;
    scanner::CodeFragment fragment;
    fragment.type = scanner::FragmentType::CHTL_JS;
    
    fragment.content = "{{button}}->addEventListener('click', handler)->focus();";
    auto result = compiler.compile(fragment);
    assert(result.success);
    // -> 应该被转换为 .
    assert(result.output.find("->") == std::string::npos);
    assert(result.output.find(".addEventListener") != std::string::npos);
    assert(result.output.find(".focus()") != std::string::npos);
    std::cout << "  ✓ Chain access transform" << std::endl;
}

void testListenMethod() {
    std::cout << "Test: listen method transformation..." << std::endl;
    
    compiler::handwritten::CHTLJSCompilerWrapper compiler;
    scanner::CodeFragment fragment;
    fragment.type = scanner::FragmentType::CHTL_JS;
    
    fragment.content = R"(
{{button}}.listen({
    click: function() { console.log('clicked'); },
    mouseenter: function() { this.style.color = 'red'; }
});
)";
    
    auto result = compiler.compile(fragment);
    assert(result.success);
    assert(result.output.find("_chtl.listen(") != std::string::npos);
    std::cout << "  ✓ listen method transform" << std::endl;
}

void testAnimateFunction() {
    std::cout << "Test: animate function transformation..." << std::endl;
    
    compiler::handwritten::CHTLJSCompilerWrapper compiler;
    scanner::CodeFragment fragment;
    fragment.type = scanner::FragmentType::CHTL_JS;
    
    fragment.content = R"(
animate({
    element: {{box}},
    duration: 1000,
    easing: 'ease-in-out',
    begin: { opacity: 0, transform: 'scale(0)' },
    when: [
        { at: 0.5, opacity: 0.5, transform: 'scale(1.1)' }
    ],
    end: { opacity: 1, transform: 'scale(1)' }
});
)";
    
    auto result = compiler.compile(fragment);
    assert(result.success);
    assert(result.output.find("_chtl.animate(") != std::string::npos);
    std::cout << "  ✓ animate function transform" << std::endl;
}

void testRuntimeFunctions() {
    std::cout << "Test: Runtime functions generation..." << std::endl;
    
    compiler::handwritten::CHTLJSCompilerWrapper compiler;
    scanner::CodeFragment fragment;
    fragment.type = scanner::FragmentType::CHTL_JS;
    
    fragment.content = "{{button}}.textContent = 'Hello';";
    auto result = compiler.compile(fragment);
    assert(result.success);
    
    // 应该包含运行时函数定义
    assert(result.output.find("const _chtl = {") != std::string::npos);
    assert(result.output.find("select: function") != std::string::npos);
    assert(result.output.find("listen: function") != std::string::npos);
    assert(result.output.find("animate: function") != std::string::npos);
    std::cout << "  ✓ Runtime functions included" << std::endl;
}

void testMixedContent() {
    std::cout << "Test: Mixed JS and CHTL JS content..." << std::endl;
    
    compiler::handwritten::CHTLJSCompilerWrapper compiler;
    scanner::CodeFragment fragment;
    fragment.type = scanner::FragmentType::CHTL_JS;
    
    fragment.content = R"(
var count = 0;
{{button}}->addEventListener('click', function() {
    count++;
    {{#counter}}.textContent = count;
});
)";
    
    auto result = compiler.compile(fragment);
    assert(result.success);
    // 应该保留原生JS代码
    assert(result.output.find("var count = 0;") != std::string::npos);
    // 应该转换CHTL JS特征
    assert(result.output.find("_chtl.select('button')") != std::string::npos);
    assert(result.output.find("_chtl.select('#counter')") != std::string::npos);
    std::cout << "  ✓ Mixed content handled correctly" << std::endl;
}

int main() {
    std::cout << "=== CHTL JS Compiler Tests ===" << std::endl;
    
    try {
        testSelectorTransform();
        testChainAccess();
        testListenMethod();
        testAnimateFunction();
        testRuntimeFunctions();
        testMixedContent();
        
        std::cout << "\n✅ All CHTL JS compiler tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ CHTL JS compiler test failed: " << e.what() << std::endl;
        return 1;
    }
}