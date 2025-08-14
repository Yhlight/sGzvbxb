#include "CHTLParserEnhanced.h"

namespace chtl {
namespace parser {

// 样式属性
std::shared_ptr<ParseContext> CHTLParserEnhanced::styleProperty() {
    auto ctx = std::make_shared<ParseContext>("styleProperty");
    
    // 检查是否是样式组继承或使用
    if (match(TokenType::AT)) {
        if (match(TokenType::KEYWORD_STYLE_GROUP)) {
            // @Style templateName;
            auto name = consume(TokenType::IDENTIFIER, "Expected style template name");
            ctx->addChild(std::make_shared<TerminalNode>(name));
            consume(TokenType::SEMICOLON, "Expected ;");
            return ctx;
        }
    } else if (match(TokenType::KEYWORD_INHERIT)) {
        // inherit @Style templateName;
        consume(TokenType::AT, "Expected @");
        consume(TokenType::KEYWORD_STYLE_GROUP, "Expected Style");
        auto name = consume(TokenType::IDENTIFIER, "Expected style template name");
        ctx->addChild(std::make_shared<TerminalNode>(name));
        consume(TokenType::SEMICOLON, "Expected ;");
        return ctx;
    }
    
    // 普通属性：property: value;
    auto property = consume(TokenType::IDENTIFIER, "Expected property name");
    ctx->addChild(std::make_shared<TerminalNode>(property));
    
    consume(TokenType::COLON, "Expected :");
    
    // 属性值 - 支持无引号字面量和 ThemeColor 函数
    if (tokens_->LT(1)->getType() == TokenType::IDENTIFIER && 
        tokens_->LT(1)->getText() == "ThemeColor" && 
        tokens_->LT(2)->getType() == TokenType::LPAREN) {
        // ThemeColor(varName) 语法
        auto themeColorToken = tokens_->consume(); // ThemeColor
        ctx->addChild(std::make_shared<TerminalNode>(themeColorToken));
        
        consume(TokenType::LPAREN, "Expected (");
        
        // 变量名
        auto varName = consume(TokenType::IDENTIFIER, "Expected variable name");
        ctx->addChild(std::make_shared<TerminalNode>(varName));
        
        // 可选的特例化: varName = value
        if (match(TokenType::EQUALS)) {
            // 特例化值
            if (tokens_->LT(1)->getType() == TokenType::STRING_LITERAL ||
                tokens_->LT(1)->getType() == TokenType::NUMBER_LITERAL ||
                tokens_->LT(1)->getType() == TokenType::IDENTIFIER) {
                ctx->addChild(std::make_shared<TerminalNode>(tokens_->consume()));
            } else {
                error("Expected value after =", tokens_->LT(1));
            }
        }
        
        consume(TokenType::RPAREN, "Expected )");
    } else if (tokens_->LT(1)->getType() == TokenType::STRING_LITERAL ||
               tokens_->LT(1)->getType() == TokenType::NUMBER_LITERAL ||
               tokens_->LT(1)->getType() == TokenType::IDENTIFIER) {
        ctx->addChild(std::make_shared<TerminalNode>(tokens_->consume()));
    } else {
        error("Expected property value", tokens_->LT(1));
    }
    
    consume(TokenType::SEMICOLON, "Expected ;");
    return ctx;
}

// 变量定义
std::shared_ptr<ParseContext> CHTLParserEnhanced::varDefinition() {
    auto ctx = std::make_shared<ParseContext>("varDefinition");
    
    // varName: value;
    auto varName = consume(TokenType::IDENTIFIER, "Expected variable name");
    ctx->addChild(std::make_shared<TerminalNode>(varName));
    
    consume(TokenType::COLON, "Expected :");
    
    // 变量值
    if (tokens_->LT(1)->getType() == TokenType::STRING_LITERAL ||
        tokens_->LT(1)->getType() == TokenType::NUMBER_LITERAL ||
        tokens_->LT(1)->getType() == TokenType::IDENTIFIER) {
        ctx->addChild(std::make_shared<TerminalNode>(tokens_->consume()));
    } else {
        error("Expected variable value", tokens_->LT(1));
    }
    
    consume(TokenType::SEMICOLON, "Expected ;");
    return ctx;
}

// [Custom] @Style
std::shared_ptr<ParseContext> CHTLParserEnhanced::customStyleGroup() {
    auto ctx = std::make_shared<ParseContext>("customStyleGroup");
    
    // @Style 已被消费
    ctx->addChild(std::make_shared<TerminalNode>(
        std::make_shared<Token>(TokenType::KEYWORD_STYLE_GROUP, "@Style", 0, 0)));
    
    // 组名
    auto name = consume(TokenType::IDENTIFIER, "Expected style group name");
    ctx->addChild(std::make_shared<TerminalNode>(name));
    
    consume(TokenType::LBRACE, "Expected {");
    
    // 自定义内容 - 支持特例化操作和无值样式
    while (!match(TokenType::RBRACE) && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
        if (match(TokenType::AT)) {
            if (match(TokenType::KEYWORD_STYLE_GROUP)) {
                // @Style templateName { 特例化操作 }
                auto templateName = consume(TokenType::IDENTIFIER, "Expected template name");
                ctx->addChild(std::make_shared<TerminalNode>(templateName));
                
                if (match(TokenType::LBRACE)) {
                    // 特例化块
                    while (!match(TokenType::RBRACE)) {
                        if (match(TokenType::KEYWORD_DELETE)) {
                            // delete property1, property2;
                            ctx->addChild(deleteOperation());
                        } else {
                            ctx->addChild(styleProperty());
                        }
                    }
                } else {
                    consume(TokenType::SEMICOLON, "Expected ; or {");
                }
            }
        } else {
            // 检查是否是无值属性（逗号分隔）
            if (tokens_->LT(1)->getType() == TokenType::IDENTIFIER &&
                (tokens_->LT(2)->getType() == TokenType::COMMA || 
                 tokens_->LT(2)->getType() == TokenType::SEMICOLON)) {
                // 无值属性列表: property1, property2;
                do {
                    auto prop = consume(TokenType::IDENTIFIER, "Expected property name");
                    ctx->addChild(std::make_shared<TerminalNode>(prop));
                } while (match(TokenType::COMMA));
                consume(TokenType::SEMICOLON, "Expected ;");
            } else {
                // 普通样式属性
                ctx->addChild(styleProperty());
            }
        }
    }
    
    return ctx;
}

// [Custom] @Element
std::shared_ptr<ParseContext> CHTLParserEnhanced::customElement() {
    auto ctx = std::make_shared<ParseContext>("customElement");
    
    // @Element 已被消费
    ctx->addChild(std::make_shared<TerminalNode>(
        std::make_shared<Token>(TokenType::KEYWORD_ELEMENT, "@Element", 0, 0)));
    
    // 元素名
    auto name = consume(TokenType::IDENTIFIER, "Expected element name");
    ctx->addChild(std::make_shared<TerminalNode>(name));
    
    consume(TokenType::LBRACE, "Expected {");
    
    // 元素内容
    while (!match(TokenType::RBRACE) && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
        ctx->addChild(htmlElement());
    }
    
    return ctx;
}

// [Custom] @Var
std::shared_ptr<ParseContext> CHTLParserEnhanced::customVarGroup() {
    auto ctx = std::make_shared<ParseContext>("customVarGroup");
    
    // @Var 已被消费
    ctx->addChild(std::make_shared<TerminalNode>(
        std::make_shared<Token>(TokenType::KEYWORD_VAR_GROUP, "@Var", 0, 0)));
    
    // 变量组名
    auto name = consume(TokenType::IDENTIFIER, "Expected var group name");
    ctx->addChild(std::make_shared<TerminalNode>(name));
    
    consume(TokenType::LBRACE, "Expected {");
    
    // 变量定义
    while (!match(TokenType::RBRACE) && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
        ctx->addChild(varDefinition());
    }
    
    return ctx;
}

// 特定导入
std::shared_ptr<ParseContext> CHTLParserEnhanced::specificImport() {
    auto ctx = std::make_shared<ParseContext>("specificImport");
    
    // [Template] 或 [Custom] 已被消费
    bool isTemplate = tokens_->LT(0)->getType() == TokenType::KEYWORD_TEMPLATE;
    
    // @Style, @Element, @Var
    consume(TokenType::AT, "Expected @");
    
    std::string importType;
    if (match(TokenType::KEYWORD_STYLE_GROUP)) {
        importType = "Style";
    } else if (match(TokenType::KEYWORD_ELEMENT)) {
        importType = "Element";
    } else if (match(TokenType::KEYWORD_VAR_GROUP)) {
        importType = "Var";
    } else {
        error("Expected Style, Element, or Var", tokens_->LT(1));
    }
    
    // 要导入的名称
    auto itemName = consume(TokenType::IDENTIFIER, "Expected item name");
    ctx->addChild(std::make_shared<TerminalNode>(itemName));
    
    // from
    consume(TokenType::KEYWORD_FROM, "Expected 'from'");
    
    // 路径
    auto path = consume(TokenType::STRING_LITERAL, "Expected file path");
    ctx->addChild(std::make_shared<TerminalNode>(path));
    
    // 可选的 as
    if (match(TokenType::KEYWORD_AS)) {
        auto alias = consume(TokenType::IDENTIFIER, "Expected alias name");
        ctx->addChild(std::make_shared<TerminalNode>(alias));
    }
    
    return ctx;
}

// 文件导入
std::shared_ptr<ParseContext> CHTLParserEnhanced::fileImport() {
    auto ctx = std::make_shared<ParseContext>("fileImport");
    
    // @ 已被消费
    std::string fileType;
    if (match(TokenType::KEYWORD_HTML)) {
        fileType = "Html";
    } else if (match(TokenType::KEYWORD_STYLE_GROUP)) {
        fileType = "Style";
    } else if (match(TokenType::KEYWORD_JAVASCRIPT)) {
        fileType = "JavaScript";
    } else if (match(TokenType::KEYWORD_CHTL)) {
        fileType = "Chtl";
    } else if (match(TokenType::KEYWORD_CJMOD)) {
        fileType = "CJmod";
    } else {
        error("Invalid import file type", tokens_->LT(1));
    }
    
    ctx->addChild(std::make_shared<TerminalNode>(
        std::make_shared<Token>(TokenType::IDENTIFIER, fileType, 0, 0)));
    
    // from
    consume(TokenType::KEYWORD_FROM, "Expected 'from'");
    
    // 路径
    auto path = consume(TokenType::STRING_LITERAL, "Expected file path");
    ctx->addChild(std::make_shared<TerminalNode>(path));
    
    // 可选的 as
    if (match(TokenType::KEYWORD_AS)) {
        auto alias = consume(TokenType::IDENTIFIER, "Expected alias name");
        ctx->addChild(std::make_shared<TerminalNode>(alias));
    }
    
    return ctx;
}

// except 约束
std::shared_ptr<ParseContext> CHTLParserEnhanced::exceptConstraint() {
    auto ctx = std::make_shared<ParseContext>("exceptConstraint");
    
    // except 已被消费
    ctx->addChild(std::make_shared<TerminalNode>(
        std::make_shared<Token>(TokenType::KEYWORD_EXCEPT, "except", 0, 0)));
    
    // 约束列表
    do {
        if (match(TokenType::AT)) {
            // @Html
            if (match(TokenType::KEYWORD_HTML)) {
                ctx->addChild(std::make_shared<TerminalNode>(
                    std::make_shared<Token>(TokenType::KEYWORD_HTML, "@Html", 0, 0)));
            } else {
                error("Expected Html after @", tokens_->LT(1));
            }
        } else if (match(TokenType::LBRACKET)) {
            // [Template] 或 [Custom]
            if (match(TokenType::KEYWORD_TEMPLATE)) {
                ctx->addChild(std::make_shared<TerminalNode>(
                    std::make_shared<Token>(TokenType::KEYWORD_TEMPLATE, "[Template]", 0, 0)));
                
                // 可选的具体类型
                if (match(TokenType::AT)) {
                    if (match(TokenType::KEYWORD_VAR_GROUP)) {
                        ctx->addChild(std::make_shared<TerminalNode>(
                            std::make_shared<Token>(TokenType::KEYWORD_VAR_GROUP, "@Var", 0, 0)));
                    }
                }
            } else if (match(TokenType::KEYWORD_CUSTOM)) {
                ctx->addChild(std::make_shared<TerminalNode>(
                    std::make_shared<Token>(TokenType::KEYWORD_CUSTOM, "[Custom]", 0, 0)));
                
                // 可选的具体类型和名称
                if (match(TokenType::AT)) {
                    if (match(TokenType::KEYWORD_ELEMENT)) {
                        ctx->addChild(std::make_shared<TerminalNode>(
                            std::make_shared<Token>(TokenType::KEYWORD_ELEMENT, "@Element", 0, 0)));
                        
                        // 具体名称
                        auto name = consume(TokenType::IDENTIFIER, "Expected element name");
                        ctx->addChild(std::make_shared<TerminalNode>(name));
                    }
                }
            }
            consume(TokenType::RBRACKET, "Expected ]");
        } else {
            // HTML 元素名
            auto elementName = consume(TokenType::IDENTIFIER, "Expected element name");
            ctx->addChild(std::make_shared<TerminalNode>(elementName));
        }
    } while (match(TokenType::COMMA));
    
    consume(TokenType::SEMICOLON, "Expected ;");
    return ctx;
}

// [Origin] @Html
std::shared_ptr<ParseContext> CHTLParserEnhanced::originHtml() {
    auto ctx = std::make_shared<ParseContext>("originHtml");
    
    // @Html 已被消费
    consume(TokenType::LBRACE, "Expected {");
    
    // 原始 HTML 内容 - 直到遇到 }
    std::string content;
    int braceCount = 1;
    
    while (braceCount > 0 && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
        auto token = tokens_->consume();
        if (token->getType() == TokenType::LBRACE) {
            braceCount++;
        } else if (token->getType() == TokenType::RBRACE) {
            braceCount--;
            if (braceCount == 0) break;
        }
        content += token->getText() + " ";
    }
    
    ctx->addChild(std::make_shared<TerminalNode>(
        std::make_shared<Token>(TokenType::TEXT_LITERAL, content, 0, 0)));
    
    return ctx;
}

// [Origin] @Style
std::shared_ptr<ParseContext> CHTLParserEnhanced::originStyle() {
    auto ctx = std::make_shared<ParseContext>("originStyle");
    
    // @Style 已被消费
    consume(TokenType::LBRACE, "Expected {");
    
    // 原始 CSS 内容
    std::string content;
    int braceCount = 1;
    
    while (braceCount > 0 && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
        auto token = tokens_->consume();
        if (token->getType() == TokenType::LBRACE) {
            braceCount++;
        } else if (token->getType() == TokenType::RBRACE) {
            braceCount--;
            if (braceCount == 0) break;
        }
        content += token->getText() + " ";
    }
    
    ctx->addChild(std::make_shared<TerminalNode>(
        std::make_shared<Token>(TokenType::TEXT_LITERAL, content, 0, 0)));
    
    return ctx;
}

// [Origin] @JavaScript
std::shared_ptr<ParseContext> CHTLParserEnhanced::originJavaScript() {
    auto ctx = std::make_shared<ParseContext>("originJavaScript");
    
    // @JavaScript 已被消费
    consume(TokenType::LBRACE, "Expected {");
    
    // 原始 JS 内容
    std::string content;
    int braceCount = 1;
    
    while (braceCount > 0 && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
        auto token = tokens_->consume();
        if (token->getType() == TokenType::LBRACE) {
            braceCount++;
        } else if (token->getType() == TokenType::RBRACE) {
            braceCount--;
            if (braceCount == 0) break;
        }
        content += token->getText() + " ";
    }
    
    ctx->addChild(std::make_shared<TerminalNode>(
        std::make_shared<Token>(TokenType::TEXT_LITERAL, content, 0, 0)));
    
    return ctx;
}

// 属性
std::shared_ptr<ParseContext> CHTLParserEnhanced::attribute() {
    auto ctx = std::make_shared<ParseContext>("attribute");
    
    // 属性名
    auto attrName = consume(TokenType::IDENTIFIER, "Expected attribute name");
    ctx->addChild(std::make_shared<TerminalNode>(attrName));
    
    // : 或 =
    if (match(TokenType::COLON)) {
        // 冒号语法
    } else if (match(TokenType::EQUALS)) {
        // 等号语法
    } else {
        error("Expected : or = after attribute name", tokens_->LT(1));
    }
    
    // 属性值 - 支持无引号字面量
    if (tokens_->LT(1)->getType() == TokenType::STRING_LITERAL ||
        tokens_->LT(1)->getType() == TokenType::NUMBER_LITERAL ||
        tokens_->LT(1)->getType() == TokenType::IDENTIFIER) {
        ctx->addChild(std::make_shared<TerminalNode>(tokens_->consume()));
    } else {
        error("Expected attribute value", tokens_->LT(1));
    }
    
    consume(TokenType::SEMICOLON, "Expected ;");
    return ctx;
}

// text 块
std::shared_ptr<ParseContext> CHTLParserEnhanced::textBlock() {
    auto ctx = std::make_shared<ParseContext>("textBlock");
    
    // text 已被消费
    consume(TokenType::LBRACE, "Expected {");
    
    // 文本内容 - 支持无引号字面量
    if (tokens_->LT(1)->getType() == TokenType::STRING_LITERAL) {
        // 引号字符串
        ctx->addChild(std::make_shared<TerminalNode>(tokens_->consume()));
    } else {
        // 无引号字面量 - 读取到 }
        std::string content;
        while (tokens_->LT(1)->getType() != TokenType::RBRACE && 
               tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
            content += tokens_->consume()->getText() + " ";
        }
        ctx->addChild(std::make_shared<TerminalNode>(
            std::make_shared<Token>(TokenType::TEXT_LITERAL, content, 0, 0)));
    }
    
    consume(TokenType::RBRACE, "Expected }");
    return ctx;
}

// 局部样式块
std::shared_ptr<ParseContext> CHTLParserEnhanced::localStyleBlock() {
    auto ctx = std::make_shared<ParseContext>("localStyleBlock");
    
    // style 已被消费
    consume(TokenType::LBRACE, "Expected {");
    
    // 样式内容
    while (!match(TokenType::RBRACE) && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
        if (match(TokenType::DOT)) {
            // .class 选择器 - 自动化类名
            auto className = consume(TokenType::IDENTIFIER, "Expected class name");
            ctx->addChild(classSelector(className));
        } else if (match(TokenType::HASH)) {
            // #id 选择器 - 自动化ID
            auto idName = consume(TokenType::IDENTIFIER, "Expected id name");
            ctx->addChild(idSelector(idName));
        } else if (match(TokenType::AMPERSAND)) {
            // & 上下文推断
            ctx->addChild(contextSelector());
        } else {
            // 内联样式
            ctx->addChild(styleProperty());
        }
    }
    
    return ctx;
}

// 局部脚本块
std::shared_ptr<ParseContext> CHTLParserEnhanced::localScriptBlock() {
    auto ctx = std::make_shared<ParseContext>("localScriptBlock");
    
    // script 已被消费
    consume(TokenType::LBRACE, "Expected {");
    
    // 脚本内容 - 原样保留
    std::string content;
    int braceCount = 1;
    
    while (braceCount > 0 && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
        auto token = tokens_->consume();
        if (token->getType() == TokenType::LBRACE) {
            braceCount++;
        } else if (token->getType() == TokenType::RBRACE) {
            braceCount--;
            if (braceCount == 0) break;
        }
        content += token->getText() + " ";
    }
    
    ctx->addChild(std::make_shared<TerminalNode>(
        std::make_shared<Token>(TokenType::TEXT_LITERAL, content, 0, 0)));
    
    return ctx;
}

// use 语句
std::shared_ptr<ParseContext> CHTLParserEnhanced::useStatement() {
    auto ctx = std::make_shared<ParseContext>("useStatement");
    
    // use 已被消费
    consume(TokenType::AT, "Expected @");
    
    if (match(TokenType::KEYWORD_STYLE_GROUP)) {
        // use @Style templateName
        auto name = consume(TokenType::IDENTIFIER, "Expected style template name");
        ctx->addChild(std::make_shared<TerminalNode>(name));
        consume(TokenType::SEMICOLON, "Expected ;");
    } else if (match(TokenType::KEYWORD_ELEMENT)) {
        // use @Element templateName { ... }
        auto name = consume(TokenType::IDENTIFIER, "Expected element template name");
        ctx->addChild(std::make_shared<TerminalNode>(name));
        
        if (match(TokenType::LBRACE)) {
            // 内容覆盖
            while (!match(TokenType::RBRACE)) {
                ctx->addChild(htmlElement());
            }
        }
    } else if (match(TokenType::KEYWORD_VAR_GROUP)) {
        // use @Var varGroup.varName
        auto groupName = consume(TokenType::IDENTIFIER, "Expected var group name");
        ctx->addChild(std::make_shared<TerminalNode>(groupName));
        
        if (match(TokenType::DOT)) {
            auto varName = consume(TokenType::IDENTIFIER, "Expected variable name");
            ctx->addChild(std::make_shared<TerminalNode>(varName));
        }
    }
    
    return ctx;
}

// insert 操作
std::shared_ptr<ParseContext> CHTLParserEnhanced::insertOperation() {
    auto ctx = std::make_shared<ParseContext>("insertOperation");
    
    // insert 已被消费
    
    // 位置关键字: after, before, replace, at top, at bottom
    std::string position;
    if (match(TokenType::KEYWORD_AFTER)) {
        position = "after";
        ctx->addChild(std::make_shared<TerminalNode>(
            std::make_shared<Token>(TokenType::KEYWORD_AFTER, "after", 0, 0)));
    } else if (match(TokenType::KEYWORD_BEFORE)) {
        position = "before";
        ctx->addChild(std::make_shared<TerminalNode>(
            std::make_shared<Token>(TokenType::KEYWORD_BEFORE, "before", 0, 0)));
    } else if (match(TokenType::KEYWORD_REPLACE)) {
        position = "replace";
        ctx->addChild(std::make_shared<TerminalNode>(
            std::make_shared<Token>(TokenType::KEYWORD_REPLACE, "replace", 0, 0)));
    } else if (match(TokenType::KEYWORD_AT)) {
        // at top 或 at bottom
        if (tokens_->LT(1)->getText() == "top") {
            consume(TokenType::IDENTIFIER, "Expected 'top'");
            position = "at top";
            ctx->addChild(std::make_shared<TerminalNode>(
                std::make_shared<Token>(TokenType::KEYWORD_AT, "at", 0, 0)));
            ctx->addChild(std::make_shared<TerminalNode>(
                std::make_shared<Token>(TokenType::KEYWORD_TOP, "top", 0, 0)));
        } else if (tokens_->LT(1)->getText() == "bottom") {
            consume(TokenType::IDENTIFIER, "Expected 'bottom'");
            position = "at bottom";
            ctx->addChild(std::make_shared<TerminalNode>(
                std::make_shared<Token>(TokenType::KEYWORD_AT, "at", 0, 0)));
            ctx->addChild(std::make_shared<TerminalNode>(
                std::make_shared<Token>(TokenType::KEYWORD_BOTTOM, "bottom", 0, 0)));
        } else {
            error("Expected 'top' or 'bottom' after 'at'", tokens_->LT(1));
        }
    }
    
    // 选择器（可选，对于 at top/bottom 不需要）
    if (position != "at top" && position != "at bottom") {
        // 元素选择器，如 div[0]
        auto selector = consume(TokenType::IDENTIFIER, "Expected element selector");
        ctx->addChild(std::make_shared<TerminalNode>(selector));
        
        // 可选的索引 [n]
        if (match(TokenType::LBRACKET)) {
            auto index = consume(TokenType::NUMBER_LITERAL, "Expected index");
            ctx->addChild(std::make_shared<TerminalNode>(index));
            consume(TokenType::RBRACKET, "Expected ]");
        }
    }
    
    // 插入的内容块
    consume(TokenType::LBRACE, "Expected {");
    
    // 插入的元素或语句
    while (!match(TokenType::RBRACE) && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
        if (match(TokenType::AT)) {
            // @Element 等使用语句
            ctx->addChild(useStatement());
        } else {
            // HTML 元素
            ctx->addChild(htmlElement());
        }
    }
    
    return ctx;
}

// 辅助方法
std::shared_ptr<ParseContext> CHTLParserEnhanced::classSelector(std::shared_ptr<Token> className) {
    auto ctx = std::make_shared<ParseContext>("classSelector");
    ctx->addChild(std::make_shared<TerminalNode>(className));
    
    consume(TokenType::LBRACE, "Expected {");
    
    // CSS 属性
    while (!match(TokenType::RBRACE)) {
        ctx->addChild(styleProperty());
    }
    
    return ctx;
}

std::shared_ptr<ParseContext> CHTLParserEnhanced::idSelector(std::shared_ptr<Token> idName) {
    auto ctx = std::make_shared<ParseContext>("idSelector");
    ctx->addChild(std::make_shared<TerminalNode>(idName));
    
    consume(TokenType::LBRACE, "Expected {");
    
    // CSS 属性
    while (!match(TokenType::RBRACE)) {
        ctx->addChild(styleProperty());
    }
    
    return ctx;
}

std::shared_ptr<ParseContext> CHTLParserEnhanced::contextSelector() {
    auto ctx = std::make_shared<ParseContext>("contextSelector");
    
    // & 已被消费
    // 可能的伪类或伪元素
    if (match(TokenType::COLON)) {
        auto pseudo = consume(TokenType::IDENTIFIER, "Expected pseudo selector");
        ctx->addChild(std::make_shared<TerminalNode>(pseudo));
    }
    
    consume(TokenType::LBRACE, "Expected {");
    
    // CSS 属性
    while (!match(TokenType::RBRACE)) {
        ctx->addChild(styleProperty());
    }
    
    return ctx;
}

std::shared_ptr<ParseContext> CHTLParserEnhanced::deleteOperation() {
    auto ctx = std::make_shared<ParseContext>("deleteOperation");
    
    // delete 已被消费
    ctx->addChild(std::make_shared<TerminalNode>(
        std::make_shared<Token>(TokenType::KEYWORD_DELETE, "delete", 0, 0)));
    
    // 属性列表
    do {
        auto prop = consume(TokenType::IDENTIFIER, "Expected property name");
        ctx->addChild(std::make_shared<TerminalNode>(prop));
    } while (match(TokenType::COMMA));
    
    consume(TokenType::SEMICOLON, "Expected ;");
    return ctx;
}

} // namespace parser
} // namespace chtl