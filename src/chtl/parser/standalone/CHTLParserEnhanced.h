#pragma once

#include "ParserRuntime.h"
#include "CHTLLexer.h"
#include <memory>
#include <vector>

namespace chtl {
namespace parser {

/**
 * 增强版 CHTL 解析器
 * 完整支持 CHTL 语法文档中的所有特性
 */
class CHTLParserEnhanced : public Parser {
public:
    CHTLParserEnhanced(std::shared_ptr<TokenStream> tokens) : Parser(tokens) {}
    
    // 主解析入口
    std::shared_ptr<ParseContext> parse() {
        return compilationUnit();
    }
    
    // 编译单元 - 文件级别
    std::shared_ptr<ParseContext> compilationUnit() {
        auto ctx = std::make_shared<ParseContext>("compilationUnit");
        
        while (tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
            // 根据 CHTL 语法文档，顶级可以有以下元素
            if (match(TokenType::KEYWORD_TEMPLATE)) {
                ctx->addChild(templateDefinition());
            } else if (match(TokenType::KEYWORD_CUSTOM)) {
                ctx->addChild(customDefinition());
            } else if (match(TokenType::KEYWORD_IMPORT)) {
                ctx->addChild(importStatement());
            } else if (match(TokenType::KEYWORD_NAMESPACE)) {
                ctx->addChild(namespaceDeclaration());
            } else if (match(TokenType::KEYWORD_ORIGIN)) {
                ctx->addChild(originDeclaration());
            } else {
                // HTML 元素
                ctx->addChild(htmlElement());
            }
        }
        
        return ctx;
    }
    
    // [Template] 定义
    std::shared_ptr<ParseContext> templateDefinition() {
        auto ctx = std::make_shared<ParseContext>("templateDefinition");
        
        // [Template] 已被消费
        ctx->addChild(std::make_shared<TerminalNode>(
            std::make_shared<Token>(TokenType::KEYWORD_TEMPLATE, "[Template]", 0, 0)));
        
        // @Style, @Element, @Var
        if (match(TokenType::KEYWORD_STYLE_GROUP)) {
            ctx->addChild(templateStyleGroup());
        } else if (match(TokenType::KEYWORD_ELEMENT)) {
            ctx->addChild(templateElement());
        } else if (match(TokenType::KEYWORD_VAR_GROUP)) {
            ctx->addChild(templateVarGroup());
        } else {
            error("Expected @Style, @Element, or @Var after [Template]", tokens_->LT(1));
        }
        
        return ctx;
    }
    
    // [Template] @Style
    std::shared_ptr<ParseContext> templateStyleGroup() {
        auto ctx = std::make_shared<ParseContext>("templateStyleGroup");
        
        // @Style 已被消费
        ctx->addChild(std::make_shared<TerminalNode>(
            std::make_shared<Token>(TokenType::KEYWORD_STYLE_GROUP, "@Style", 0, 0)));
        
        // 组名
        auto name = consume(TokenType::IDENTIFIER, "Expected style group name");
        ctx->addChild(std::make_shared<TerminalNode>(name));
        
        // 可选继承
        if (match(TokenType::COLON)) {
            auto baseName = consume(TokenType::IDENTIFIER, "Expected base template name");
            ctx->addChild(std::make_shared<TerminalNode>(baseName));
        }
        
        consume(TokenType::LBRACE, "Expected {");
        
        // 样式内容
        while (!match(TokenType::RBRACE) && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
            ctx->addChild(styleProperty());
        }
        
        return ctx;
    }
    
    // [Template] @Element
    std::shared_ptr<ParseContext> templateElement() {
        auto ctx = std::make_shared<ParseContext>("templateElement");
        
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
    
    // [Template] @Var
    std::shared_ptr<ParseContext> templateVarGroup() {
        auto ctx = std::make_shared<ParseContext>("templateVarGroup");
        
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
    
    // [Custom] 定义
    std::shared_ptr<ParseContext> customDefinition() {
        auto ctx = std::make_shared<ParseContext>("customDefinition");
        
        // [Custom] 已被消费
        ctx->addChild(std::make_shared<TerminalNode>(
            std::make_shared<Token>(TokenType::KEYWORD_CUSTOM, "[Custom]", 0, 0)));
        
        // @Style, @Element, @Var
        if (match(TokenType::KEYWORD_STYLE_GROUP)) {
            ctx->addChild(customStyleGroup());
        } else if (match(TokenType::KEYWORD_ELEMENT)) {
            ctx->addChild(customElement());
        } else if (match(TokenType::KEYWORD_VAR_GROUP)) {
            ctx->addChild(customVarGroup());
        } else {
            error("Expected @Style, @Element, or @Var after [Custom]", tokens_->LT(1));
        }
        
        return ctx;
    }
    
    // [Import] 语句
    std::shared_ptr<ParseContext> importStatement() {
        auto ctx = std::make_shared<ParseContext>("importStatement");
        
        // [Import] 已被消费
        ctx->addChild(std::make_shared<TerminalNode>(
            std::make_shared<Token>(TokenType::KEYWORD_IMPORT, "[Import]", 0, 0)));
        
        // 判断导入类型
        if (match(TokenType::KEYWORD_TEMPLATE) || match(TokenType::KEYWORD_CUSTOM)) {
            // 特定导入：[Import] [Template/Custom] @Type name from "path"
            ctx->addChild(specificImport());
        } else if (match(TokenType::AT)) {
            // 文件导入：[Import] @Html/Style/JavaScript/Chtl/CJmod from "path"
            ctx->addChild(fileImport());
        } else {
            error("Invalid import syntax", tokens_->LT(1));
        }
        
        return ctx;
    }
    
    // [Namespace] 声明
    std::shared_ptr<ParseContext> namespaceDeclaration() {
        auto ctx = std::make_shared<ParseContext>("namespaceDeclaration");
        
        // [Namespace] 已被消费
        ctx->addChild(std::make_shared<TerminalNode>(
            std::make_shared<Token>(TokenType::KEYWORD_NAMESPACE, "[Namespace]", 0, 0)));
        
        // 命名空间名称
        auto name = consume(TokenType::IDENTIFIER, "Expected namespace name");
        ctx->addChild(std::make_shared<TerminalNode>(name));
        
        consume(TokenType::LBRACE, "Expected {");
        
        // 命名空间内容
        while (!match(TokenType::RBRACE) && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
            if (match(TokenType::KEYWORD_TEMPLATE)) {
                ctx->addChild(templateDefinition());
            } else if (match(TokenType::KEYWORD_CUSTOM)) {
                ctx->addChild(customDefinition());
            } else if (match(TokenType::KEYWORD_NAMESPACE)) {
                // 嵌套命名空间
                ctx->addChild(namespaceDeclaration());
            } else if (match(TokenType::KEYWORD_EXCEPT)) {
                // 全局约束
                ctx->addChild(exceptConstraint());
            }
        }
        
        return ctx;
    }
    
    // [Origin] 原始嵌入
    std::shared_ptr<ParseContext> originDeclaration() {
        auto ctx = std::make_shared<ParseContext>("originDeclaration");
        
        // [Origin] 已被消费
        ctx->addChild(std::make_shared<TerminalNode>(
            std::make_shared<Token>(TokenType::KEYWORD_ORIGIN, "[Origin]", 0, 0)));
        
        // @Html, @Style, @JavaScript
        if (match(TokenType::KEYWORD_HTML)) {
            ctx->addChild(originHtml());
        } else if (match(TokenType::KEYWORD_STYLE_GROUP)) {
            ctx->addChild(originStyle());
        } else if (match(TokenType::KEYWORD_JAVASCRIPT)) {
            ctx->addChild(originJavaScript());
        } else {
            error("Expected @Html, @Style, or @JavaScript after [Origin]", tokens_->LT(1));
        }
        
        return ctx;
    }
    
    // HTML 元素
    std::shared_ptr<ParseContext> htmlElement() {
        auto ctx = std::make_shared<ParseContext>("htmlElement");
        
        // 元素名
        auto tagName = consume(TokenType::IDENTIFIER, "Expected element name");
        ctx->addChild(std::make_shared<TerminalNode>(tagName));
        
        consume(TokenType::LBRACE, "Expected {");
        
        // 元素内容
        while (!match(TokenType::RBRACE) && tokens_->LT(1)->getType() != TokenType::EOF_TOKEN) {
            if (tokens_->LT(1)->getType() == TokenType::IDENTIFIER && 
                (tokens_->LT(2)->getType() == TokenType::COLON || 
                 tokens_->LT(2)->getType() == TokenType::EQUALS)) {
                // 属性
                ctx->addChild(attribute());
            } else if (match(TokenType::KEYWORD_TEXT)) {
                // text 块
                ctx->addChild(textBlock());
            } else if (match(TokenType::KEYWORD_STYLE)) {
                // 局部样式块
                ctx->addChild(localStyleBlock());
            } else if (match(TokenType::KEYWORD_SCRIPT)) {
                // 局部脚本块
                ctx->addChild(localScriptBlock());
            } else if (match(TokenType::KEYWORD_EXCEPT)) {
                // except 约束
                ctx->addChild(exceptConstraint());
            } else if (match(TokenType::KEYWORD_USE)) {
                // use 语句
                ctx->addChild(useStatement());
            } else if (match(TokenType::KEYWORD_INSERT)) {
                // insert 操作
                ctx->addChild(insertOperation());
            } else if (match(TokenType::KEYWORD_ORIGIN)) {
                // [Origin] 块
                ctx->addChild(originDeclaration());
            } else if (match(TokenType::AT)) {
                // @Element templateName; 或 @Style templateName;
                if (match(TokenType::KEYWORD_ELEMENT)) {
                    auto name = consume(TokenType::IDENTIFIER, "Expected element template name");
                    ctx->addChild(std::make_shared<TerminalNode>(name));
                    
                    // 可选的 from namespace
                    if (match(TokenType::KEYWORD_FROM)) {
                        auto nsName = consume(TokenType::IDENTIFIER, "Expected namespace name");
                        ctx->addChild(std::make_shared<TerminalNode>(nsName));
                        
                        // 嵌套命名空间: space.room.room2
                        while (match(TokenType::DOT)) {
                            auto nestedNs = consume(TokenType::IDENTIFIER, "Expected namespace name");
                            ctx->addChild(std::make_shared<TerminalNode>(nestedNs));
                        }
                    }
                    
                    // 可选的内容覆盖
                    if (match(TokenType::LBRACE)) {
                        while (!match(TokenType::RBRACE)) {
                            ctx->addChild(htmlElement());
                        }
                    } else {
                        consume(TokenType::SEMICOLON, "Expected ;");
                    }
                } else if (match(TokenType::KEYWORD_STYLE_GROUP)) {
                    auto name = consume(TokenType::IDENTIFIER, "Expected style template name");
                    ctx->addChild(std::make_shared<TerminalNode>(name));
                    
                    // 可选的 from namespace
                    if (match(TokenType::KEYWORD_FROM)) {
                        auto nsName = consume(TokenType::IDENTIFIER, "Expected namespace name");
                        ctx->addChild(std::make_shared<TerminalNode>(nsName));
                        
                        // 嵌套命名空间
                        while (match(TokenType::DOT)) {
                            auto nestedNs = consume(TokenType::IDENTIFIER, "Expected namespace name");
                            ctx->addChild(std::make_shared<TerminalNode>(nestedNs));
                        }
                    }
                    
                    consume(TokenType::SEMICOLON, "Expected ;");
                } else {
                    error("Expected @Element or @Style after @", tokens_->LT(1));
                }
            } else {
                // 嵌套元素
                ctx->addChild(htmlElement());
            }
        }
        
        return ctx;
    }
    
private:
    // 辅助方法实现...
    std::shared_ptr<ParseContext> styleProperty();
    std::shared_ptr<ParseContext> varDefinition();
    std::shared_ptr<ParseContext> customStyleGroup();
    std::shared_ptr<ParseContext> customElement();
    std::shared_ptr<ParseContext> customVarGroup();
    std::shared_ptr<ParseContext> specificImport();
    std::shared_ptr<ParseContext> fileImport();
    std::shared_ptr<ParseContext> exceptConstraint();
    std::shared_ptr<ParseContext> originHtml();
    std::shared_ptr<ParseContext> originStyle();
    std::shared_ptr<ParseContext> originJavaScript();
    std::shared_ptr<ParseContext> attribute();
    std::shared_ptr<ParseContext> textBlock();
    std::shared_ptr<ParseContext> localStyleBlock();
    std::shared_ptr<ParseContext> localScriptBlock();
    std::shared_ptr<ParseContext> useStatement();
    std::shared_ptr<ParseContext> insertOperation();
    
    // 额外的辅助方法
    std::shared_ptr<ParseContext> classSelector(std::shared_ptr<Token> className);
    std::shared_ptr<ParseContext> idSelector(std::shared_ptr<Token> idName);
    std::shared_ptr<ParseContext> contextSelector();
    std::shared_ptr<ParseContext> deleteOperation();
};

} // namespace parser
} // namespace chtl