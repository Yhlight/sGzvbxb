#include <gtest/gtest.h>
#include "../../src/chtl/scanner/CHTLUnifiedScanner.h"
#include <string>

using namespace chtl::scanner;

class UnifiedScannerTest : public ::testing::Test {
protected:
    CHTLUnifiedScanner scanner;
    
    std::string getFragmentTypeString(FragmentType type) {
        switch(type) {
            case FragmentType::CHTL: return "CHTL";
            case FragmentType::CHTL_JS: return "CHTL_JS";
            case FragmentType::CSS: return "CSS";
            case FragmentType::JAVASCRIPT: return "JAVASCRIPT";
            default: return "UNKNOWN";
        }
    }
};

TEST_F(UnifiedScannerTest, EmptyInput) {
    auto fragments = scanner.scan("");
    EXPECT_TRUE(fragments.empty());
}

TEST_F(UnifiedScannerTest, SimpleCHTL) {
    std::string input = R"(
body {
    text "Hello World"
}
)";
    
    auto fragments = scanner.scan(input);
    ASSERT_EQ(fragments.size(), 1);
    EXPECT_EQ(fragments[0].type, FragmentType::CHTL);
    EXPECT_EQ(fragments[0].content, input);
}

TEST_F(UnifiedScannerTest, GlobalStyleBlock) {
    std::string input = R"(
style {
    body {
        color: red;
    }
}
)";
    
    auto fragments = scanner.scan(input);
    ASSERT_GE(fragments.size(), 2);
    
    // 第一个片段应该是 "style {"
    EXPECT_EQ(fragments[0].type, FragmentType::CHTL);
    EXPECT_TRUE(fragments[0].content.find("style {") != std::string::npos);
    
    // 第二个片段应该是CSS内容
    EXPECT_EQ(fragments[1].type, FragmentType::CSS);
    EXPECT_TRUE(fragments[1].content.find("body") != std::string::npos);
}

TEST_F(UnifiedScannerTest, LocalStyleBlock) {
    std::string input = R"(
body {
    style {
        background: blue;
    }
}
)";
    
    auto fragments = scanner.scan(input);
    // 局部样式块应该作为CHTL整体处理
    ASSERT_EQ(fragments.size(), 1);
    EXPECT_EQ(fragments[0].type, FragmentType::CHTL);
}

TEST_F(UnifiedScannerTest, CHTLJSFeatures) {
    std::string input = R"(
script {
    console.log("start");
    {{.btn}}.listen({
        click: () => {
            console.log("clicked");
        }
    });
    console.log("end");
}
)";
    
    auto fragments = scanner.scan(input);
    ASSERT_GE(fragments.size(), 3);
    
    bool hasCHTLJS = false;
    bool hasJS = false;
    
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL_JS) {
            hasCHTLJS = true;
            // 应该包含完整的 {{.btn}}.listen(...) 
            EXPECT_TRUE(fragment.content.find("{{.btn}}") != std::string::npos);
            EXPECT_TRUE(fragment.content.find("listen") != std::string::npos);
        } else if (fragment.type == FragmentType::JAVASCRIPT) {
            hasJS = true;
        }
    }
    
    EXPECT_TRUE(hasCHTLJS) << "Should have CHTL_JS fragments";
    EXPECT_TRUE(hasJS) << "Should have JavaScript fragments";
}

TEST_F(UnifiedScannerTest, CHTLJSChainAccess) {
    std::string input = R"(
script {
    {{#app}}->textContent = "Hello";
}
)";
    
    auto fragments = scanner.scan(input);
    
    bool foundCHTLJS = false;
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL_JS) {
            foundCHTLJS = true;
            // 应该包含 {{#app}}->textContent 作为一个完整片段
            EXPECT_TRUE(fragment.content.find("{{#app}}") != std::string::npos);
            EXPECT_TRUE(fragment.content.find("->") != std::string::npos);
            EXPECT_TRUE(fragment.content.find("textContent") != std::string::npos);
        }
    }
    
    EXPECT_TRUE(foundCHTLJS) << "Should find CHTL_JS fragment with chain access";
}

TEST_F(UnifiedScannerTest, MixedContent) {
    std::string input = R"(
// 全局样式
style {
    @Var primary-color: #333;
    body { color: var(primary-color); }
}

body {
    // 局部样式
    style {
        margin: 0;
    }
    
    div {
        text "Hello"
        
        script {
            {{.box}}.animate({opacity: 1}, 300);
        }
    }
}
)";
    
    auto fragments = scanner.scan(input);
    
    int chtlCount = 0, cssCount = 0, chtljsCount = 0, jsCount = 0;
    
    for (const auto& fragment : fragments) {
        switch(fragment.type) {
            case FragmentType::CHTL: chtlCount++; break;
            case FragmentType::CSS: cssCount++; break;
            case FragmentType::CHTL_JS: chtljsCount++; break;
            case FragmentType::JAVASCRIPT: jsCount++; break;
        }
    }
    
    EXPECT_GT(chtlCount, 0) << "Should have CHTL fragments";
    EXPECT_GT(cssCount, 0) << "Should have CSS fragments for global style";
    EXPECT_GT(chtljsCount, 0) << "Should have CHTL_JS fragments";
}

TEST_F(UnifiedScannerTest, CommentHandling) {
    std::string input = R"(
// Single line comment
/*
 * Multi-line comment
 */
body {
    text "Hello"
}
)";
    
    auto fragments = scanner.scan(input);
    ASSERT_GE(fragments.size(), 1);
    
    // 注释应该被包含在CHTL片段中
    EXPECT_TRUE(fragments[0].content.find("//") != std::string::npos);
    EXPECT_TRUE(fragments[0].content.find("/*") != std::string::npos);
}

TEST_F(UnifiedScannerTest, StringHandling) {
    std::string input = R"(
script {
    let msg = "This has {{ but it's in a string }}";
    console.log(msg);
}
)";
    
    auto fragments = scanner.scan(input);
    
    // 字符串中的 {{ 不应该被识别为 CHTL_JS
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::JAVASCRIPT) {
            // JavaScript片段应该包含完整的字符串
            EXPECT_TRUE(fragment.content.find("This has {{ but it's in a string }}") != std::string::npos);
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}