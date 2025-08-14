#include <gtest/gtest.h>
#include "../../src/chtl/compiler/CompilerDispatcher.h"
#include <string>

using namespace chtl::compiler;

class CompilerDispatcherTest : public ::testing::Test {
protected:
    CompilerDispatcher dispatcher;
};

TEST_F(CompilerDispatcherTest, EmptyInput) {
    auto result = dispatcher.compile("");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.html.empty());
    EXPECT_TRUE(result.css.empty());
    EXPECT_TRUE(result.javascript.empty());
}

TEST_F(CompilerDispatcherTest, SimpleCHTL) {
    std::string input = R"(
body {
    h1 {
        text "Hello World"
    }
}
)";
    
    auto result = dispatcher.compile(input);
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.html.empty());
    
    // 应该生成HTML结构
    EXPECT_TRUE(result.html.find("<body>") != std::string::npos);
    EXPECT_TRUE(result.html.find("<h1>") != std::string::npos);
    EXPECT_TRUE(result.html.find("Hello World") != std::string::npos);
}

TEST_F(CompilerDispatcherTest, WithGlobalStyles) {
    std::string input = R"(
style {
    body {
        margin: 0;
        padding: 0;
    }
    .container {
        width: 100%;
    }
}

body {
    div {
        class: "container";
        text "Content"
    }
}
)";
    
    auto result = dispatcher.compile(input);
    EXPECT_TRUE(result.success);
    
    // 检查CSS生成
    EXPECT_FALSE(result.css.empty());
    EXPECT_TRUE(result.css.find("margin: 0") != std::string::npos);
    EXPECT_TRUE(result.css.find(".container") != std::string::npos);
    
    // 检查HTML生成
    EXPECT_TRUE(result.html.find("class=\"container\"") != std::string::npos);
}

TEST_F(CompilerDispatcherTest, WithLocalStyles) {
    std::string input = R"(
body {
    div {
        style {
            background: blue;
            color: white;
        }
        text "Styled content"
    }
}
)";
    
    auto result = dispatcher.compile(input);
    EXPECT_TRUE(result.success);
    
    // 局部样式应该生成内联样式或特定的CSS类
    EXPECT_TRUE(result.html.find("style=") != std::string::npos || 
                result.css.find("background: blue") != std::string::npos);
}

TEST_F(CompilerDispatcherTest, WithCHTLJS) {
    std::string input = R"(
body {
    button {
        id: "myBtn";
        text "Click me"
    }
    
    script {
        {{#myBtn}}.listen({
            click: () => {
                alert("Clicked!");
            }
        });
    }
}
)";
    
    auto result = dispatcher.compile(input);
    EXPECT_TRUE(result.success);
    
    // 检查JavaScript生成
    EXPECT_FALSE(result.javascript.empty());
    
    // CHTL JS应该被转换为普通JavaScript
    EXPECT_TRUE(result.javascript.find("document.getElementById") != std::string::npos ||
                result.javascript.find("querySelector") != std::string::npos);
    EXPECT_TRUE(result.javascript.find("addEventListener") != std::string::npos);
}

TEST_F(CompilerDispatcherTest, MixedJavaScript) {
    std::string input = R"(
script {
    // 普通JavaScript
    function greet(name) {
        return "Hello, " + name;
    }
    
    // CHTL JS
    {{.btn}}->textContent = greet("World");
}
)";
    
    auto result = dispatcher.compile(input);
    EXPECT_TRUE(result.success);
    
    // 应该包含普通JavaScript函数
    EXPECT_TRUE(result.javascript.find("function greet") != std::string::npos);
    
    // CHTL JS应该被转换
    EXPECT_TRUE(result.javascript.find("textContent") != std::string::npos);
}

TEST_F(CompilerDispatcherTest, ComplexExample) {
    std::string input = R"(
// 全局样式
style {
    * {
        box-sizing: border-box;
    }
    body {
        font-family: Arial, sans-serif;
    }
}

body {
    header {
        style {
            background: #333;
            color: white;
            padding: 1rem;
        }
        
        h1 {
            text "My App"
        }
    }
    
    main {
        class: "content";
        
        button {
            id: "actionBtn";
            class: "btn primary";
            text "Action"
            
            script {
                {{#actionBtn}}.listen({
                    click: () => {
                        {{.content}}->innerHTML += "<p>New paragraph</p>";
                    }
                });
            }
        }
    }
    
    script {
        console.log("App initialized");
    }
}
)";
    
    auto result = dispatcher.compile(input);
    EXPECT_TRUE(result.success);
    
    // 验证所有部分都生成了
    EXPECT_FALSE(result.html.empty());
    EXPECT_FALSE(result.css.empty());
    EXPECT_FALSE(result.javascript.empty());
    
    // HTML结构
    EXPECT_TRUE(result.html.find("<header>") != std::string::npos);
    EXPECT_TRUE(result.html.find("<main") != std::string::npos);
    EXPECT_TRUE(result.html.find("class=\"content\"") != std::string::npos);
    
    // CSS内容
    EXPECT_TRUE(result.css.find("box-sizing: border-box") != std::string::npos);
    
    // JavaScript内容
    EXPECT_TRUE(result.javascript.find("console.log") != std::string::npos);
}

TEST_F(CompilerDispatcherTest, ErrorHandling) {
    // 测试语法错误
    std::string input = R"(
body {
    div {
        // 缺少引号的属性值
        class: invalid syntax
    }
}
)";
    
    auto result = dispatcher.compile(input);
    // 根据实现，可能会失败或生成部分结果
    // 这里只验证不会崩溃
    EXPECT_NO_THROW({
        dispatcher.compile(input);
    });
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}