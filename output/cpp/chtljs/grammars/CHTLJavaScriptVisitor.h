
// Generated from grammars/CHTLJavaScript.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLJavaScriptParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by CHTLJavaScriptParser.
 */
class  CHTLJavaScriptVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by CHTLJavaScriptParser.
   */
    virtual std::any visitPrimaryExpression(CHTLJavaScriptParser::PrimaryExpressionContext *context) = 0;

    virtual std::any visitChtlSelector(CHTLJavaScriptParser::ChtlSelectorContext *context) = 0;

    virtual std::any visitSelectorExpression(CHTLJavaScriptParser::SelectorExpressionContext *context) = 0;

    virtual std::any visitSelectorIdentifier(CHTLJavaScriptParser::SelectorIdentifierContext *context) = 0;

    virtual std::any visitSelectorIndex(CHTLJavaScriptParser::SelectorIndexContext *context) = 0;

    virtual std::any visitComplexSelector(CHTLJavaScriptParser::ComplexSelectorContext *context) = 0;

    virtual std::any visitSelectorPart(CHTLJavaScriptParser::SelectorPartContext *context) = 0;

    virtual std::any visitMemberExpression(CHTLJavaScriptParser::MemberExpressionContext *context) = 0;

    virtual std::any visitCallExpression(CHTLJavaScriptParser::CallExpressionContext *context) = 0;

    virtual std::any visitAnimateCall(CHTLJavaScriptParser::AnimateCallContext *context) = 0;

    virtual std::any visitListenConfig(CHTLJavaScriptParser::ListenConfigContext *context) = 0;

    virtual std::any visitEventHandlerList(CHTLJavaScriptParser::EventHandlerListContext *context) = 0;

    virtual std::any visitEventHandler(CHTLJavaScriptParser::EventHandlerContext *context) = 0;

    virtual std::any visitDelegateConfig(CHTLJavaScriptParser::DelegateConfigContext *context) = 0;

    virtual std::any visitDelegateProperty(CHTLJavaScriptParser::DelegatePropertyContext *context) = 0;

    virtual std::any visitSelectorArray(CHTLJavaScriptParser::SelectorArrayContext *context) = 0;

    virtual std::any visitAnimateConfig(CHTLJavaScriptParser::AnimateConfigContext *context) = 0;

    virtual std::any visitAnimateProperty(CHTLJavaScriptParser::AnimatePropertyContext *context) = 0;

    virtual std::any visitCssBlock(CHTLJavaScriptParser::CssBlockContext *context) = 0;

    virtual std::any visitCssProperty(CHTLJavaScriptParser::CssPropertyContext *context) = 0;

    virtual std::any visitPropertyName(CHTLJavaScriptParser::PropertyNameContext *context) = 0;

    virtual std::any visitPropertyValue(CHTLJavaScriptParser::PropertyValueContext *context) = 0;

    virtual std::any visitCssFunction(CHTLJavaScriptParser::CssFunctionContext *context) = 0;

    virtual std::any visitCssFunctionArguments(CHTLJavaScriptParser::CssFunctionArgumentsContext *context) = 0;

    virtual std::any visitWhenArray(CHTLJavaScriptParser::WhenArrayContext *context) = 0;

    virtual std::any visitWhenItem(CHTLJavaScriptParser::WhenItemContext *context) = 0;

    virtual std::any visitWhenProperty(CHTLJavaScriptParser::WhenPropertyContext *context) = 0;

    virtual std::any visitLiteral(CHTLJavaScriptParser::LiteralContext *context) = 0;

    virtual std::any visitProgram(CHTLJavaScriptParser::ProgramContext *context) = 0;

    virtual std::any visitSourceElements(CHTLJavaScriptParser::SourceElementsContext *context) = 0;

    virtual std::any visitSourceElement(CHTLJavaScriptParser::SourceElementContext *context) = 0;

    virtual std::any visitDeclaration(CHTLJavaScriptParser::DeclarationContext *context) = 0;

    virtual std::any visitHoistableDeclaration(CHTLJavaScriptParser::HoistableDeclarationContext *context) = 0;

    virtual std::any visitFunctionDeclaration(CHTLJavaScriptParser::FunctionDeclarationContext *context) = 0;

    virtual std::any visitGeneratorDeclaration(CHTLJavaScriptParser::GeneratorDeclarationContext *context) = 0;

    virtual std::any visitAsyncFunctionDeclaration(CHTLJavaScriptParser::AsyncFunctionDeclarationContext *context) = 0;

    virtual std::any visitAsyncGeneratorDeclaration(CHTLJavaScriptParser::AsyncGeneratorDeclarationContext *context) = 0;

    virtual std::any visitClassDeclaration(CHTLJavaScriptParser::ClassDeclarationContext *context) = 0;

    virtual std::any visitClassExtends(CHTLJavaScriptParser::ClassExtendsContext *context) = 0;

    virtual std::any visitClassBody(CHTLJavaScriptParser::ClassBodyContext *context) = 0;

    virtual std::any visitClassElement(CHTLJavaScriptParser::ClassElementContext *context) = 0;

    virtual std::any visitMethodDefinition(CHTLJavaScriptParser::MethodDefinitionContext *context) = 0;

    virtual std::any visitFieldDefinition(CHTLJavaScriptParser::FieldDefinitionContext *context) = 0;

    virtual std::any visitLexicalDeclaration(CHTLJavaScriptParser::LexicalDeclarationContext *context) = 0;

    virtual std::any visitLetOrConst(CHTLJavaScriptParser::LetOrConstContext *context) = 0;

    virtual std::any visitBindingList(CHTLJavaScriptParser::BindingListContext *context) = 0;

    virtual std::any visitLexicalBinding(CHTLJavaScriptParser::LexicalBindingContext *context) = 0;

    virtual std::any visitStatement(CHTLJavaScriptParser::StatementContext *context) = 0;

    virtual std::any visitBlockStatement(CHTLJavaScriptParser::BlockStatementContext *context) = 0;

    virtual std::any visitStatementList(CHTLJavaScriptParser::StatementListContext *context) = 0;

    virtual std::any visitVariableStatement(CHTLJavaScriptParser::VariableStatementContext *context) = 0;

    virtual std::any visitVariableDeclarationList(CHTLJavaScriptParser::VariableDeclarationListContext *context) = 0;

    virtual std::any visitVariableDeclaration(CHTLJavaScriptParser::VariableDeclarationContext *context) = 0;

    virtual std::any visitEmptyStatement(CHTLJavaScriptParser::EmptyStatementContext *context) = 0;

    virtual std::any visitExpressionStatement(CHTLJavaScriptParser::ExpressionStatementContext *context) = 0;

    virtual std::any visitIfStatement(CHTLJavaScriptParser::IfStatementContext *context) = 0;

    virtual std::any visitIterationStatement(CHTLJavaScriptParser::IterationStatementContext *context) = 0;

    virtual std::any visitDoWhileStatement(CHTLJavaScriptParser::DoWhileStatementContext *context) = 0;

    virtual std::any visitWhileStatement(CHTLJavaScriptParser::WhileStatementContext *context) = 0;

    virtual std::any visitForStatement(CHTLJavaScriptParser::ForStatementContext *context) = 0;

    virtual std::any visitForInit(CHTLJavaScriptParser::ForInitContext *context) = 0;

    virtual std::any visitForInStatement(CHTLJavaScriptParser::ForInStatementContext *context) = 0;

    virtual std::any visitForOfStatement(CHTLJavaScriptParser::ForOfStatementContext *context) = 0;

    virtual std::any visitForInBinding(CHTLJavaScriptParser::ForInBindingContext *context) = 0;

    virtual std::any visitContinueStatement(CHTLJavaScriptParser::ContinueStatementContext *context) = 0;

    virtual std::any visitBreakStatement(CHTLJavaScriptParser::BreakStatementContext *context) = 0;

    virtual std::any visitReturnStatement(CHTLJavaScriptParser::ReturnStatementContext *context) = 0;

    virtual std::any visitWithStatement(CHTLJavaScriptParser::WithStatementContext *context) = 0;

    virtual std::any visitLabelledStatement(CHTLJavaScriptParser::LabelledStatementContext *context) = 0;

    virtual std::any visitSwitchStatement(CHTLJavaScriptParser::SwitchStatementContext *context) = 0;

    virtual std::any visitCaseClause(CHTLJavaScriptParser::CaseClauseContext *context) = 0;

    virtual std::any visitDefaultClause(CHTLJavaScriptParser::DefaultClauseContext *context) = 0;

    virtual std::any visitThrowStatement(CHTLJavaScriptParser::ThrowStatementContext *context) = 0;

    virtual std::any visitTryStatement(CHTLJavaScriptParser::TryStatementContext *context) = 0;

    virtual std::any visitCatchClause(CHTLJavaScriptParser::CatchClauseContext *context) = 0;

    virtual std::any visitCatchParameter(CHTLJavaScriptParser::CatchParameterContext *context) = 0;

    virtual std::any visitFinallyClause(CHTLJavaScriptParser::FinallyClauseContext *context) = 0;

    virtual std::any visitDebuggerStatement(CHTLJavaScriptParser::DebuggerStatementContext *context) = 0;

    virtual std::any visitImportStatement(CHTLJavaScriptParser::ImportStatementContext *context) = 0;

    virtual std::any visitImportClause(CHTLJavaScriptParser::ImportClauseContext *context) = 0;

    virtual std::any visitImportedDefaultBinding(CHTLJavaScriptParser::ImportedDefaultBindingContext *context) = 0;

    virtual std::any visitNamespaceImport(CHTLJavaScriptParser::NamespaceImportContext *context) = 0;

    virtual std::any visitNamedImports(CHTLJavaScriptParser::NamedImportsContext *context) = 0;

    virtual std::any visitImportsList(CHTLJavaScriptParser::ImportsListContext *context) = 0;

    virtual std::any visitImportSpecifier(CHTLJavaScriptParser::ImportSpecifierContext *context) = 0;

    virtual std::any visitModuleSpecifier(CHTLJavaScriptParser::ModuleSpecifierContext *context) = 0;

    virtual std::any visitExportStatement(CHTLJavaScriptParser::ExportStatementContext *context) = 0;

    virtual std::any visitExportClause(CHTLJavaScriptParser::ExportClauseContext *context) = 0;

    virtual std::any visitExportsList(CHTLJavaScriptParser::ExportsListContext *context) = 0;

    virtual std::any visitExportSpecifier(CHTLJavaScriptParser::ExportSpecifierContext *context) = 0;

    virtual std::any visitExpression(CHTLJavaScriptParser::ExpressionContext *context) = 0;

    virtual std::any visitAssignmentExpression(CHTLJavaScriptParser::AssignmentExpressionContext *context) = 0;

    virtual std::any visitAssignmentOperator(CHTLJavaScriptParser::AssignmentOperatorContext *context) = 0;

    virtual std::any visitConditionalExpression(CHTLJavaScriptParser::ConditionalExpressionContext *context) = 0;

    virtual std::any visitCoalesceExpression(CHTLJavaScriptParser::CoalesceExpressionContext *context) = 0;

    virtual std::any visitLogicalORExpression(CHTLJavaScriptParser::LogicalORExpressionContext *context) = 0;

    virtual std::any visitLogicalANDExpression(CHTLJavaScriptParser::LogicalANDExpressionContext *context) = 0;

    virtual std::any visitBitwiseORExpression(CHTLJavaScriptParser::BitwiseORExpressionContext *context) = 0;

    virtual std::any visitBitwiseXORExpression(CHTLJavaScriptParser::BitwiseXORExpressionContext *context) = 0;

    virtual std::any visitBitwiseANDExpression(CHTLJavaScriptParser::BitwiseANDExpressionContext *context) = 0;

    virtual std::any visitEqualityExpression(CHTLJavaScriptParser::EqualityExpressionContext *context) = 0;

    virtual std::any visitRelationalExpression(CHTLJavaScriptParser::RelationalExpressionContext *context) = 0;

    virtual std::any visitShiftExpression(CHTLJavaScriptParser::ShiftExpressionContext *context) = 0;

    virtual std::any visitAdditiveExpression(CHTLJavaScriptParser::AdditiveExpressionContext *context) = 0;

    virtual std::any visitMultiplicativeExpression(CHTLJavaScriptParser::MultiplicativeExpressionContext *context) = 0;

    virtual std::any visitExponentiationExpression(CHTLJavaScriptParser::ExponentiationExpressionContext *context) = 0;

    virtual std::any visitUnaryExpression(CHTLJavaScriptParser::UnaryExpressionContext *context) = 0;

    virtual std::any visitUpdateExpression(CHTLJavaScriptParser::UpdateExpressionContext *context) = 0;

    virtual std::any visitLeftHandSideExpression(CHTLJavaScriptParser::LeftHandSideExpressionContext *context) = 0;

    virtual std::any visitNewExpression(CHTLJavaScriptParser::NewExpressionContext *context) = 0;

    virtual std::any visitSuperCall(CHTLJavaScriptParser::SuperCallContext *context) = 0;

    virtual std::any visitImportCall(CHTLJavaScriptParser::ImportCallContext *context) = 0;

    virtual std::any visitArguments(CHTLJavaScriptParser::ArgumentsContext *context) = 0;

    virtual std::any visitArgumentList(CHTLJavaScriptParser::ArgumentListContext *context) = 0;

    virtual std::any visitArgument(CHTLJavaScriptParser::ArgumentContext *context) = 0;

    virtual std::any visitMetaProperty(CHTLJavaScriptParser::MetaPropertyContext *context) = 0;

    virtual std::any visitParenthesizedExpression(CHTLJavaScriptParser::ParenthesizedExpressionContext *context) = 0;

    virtual std::any visitNullLiteral(CHTLJavaScriptParser::NullLiteralContext *context) = 0;

    virtual std::any visitBooleanLiteral(CHTLJavaScriptParser::BooleanLiteralContext *context) = 0;

    virtual std::any visitNumericLiteral(CHTLJavaScriptParser::NumericLiteralContext *context) = 0;

    virtual std::any visitStringLiteral(CHTLJavaScriptParser::StringLiteralContext *context) = 0;

    virtual std::any visitArrayLiteral(CHTLJavaScriptParser::ArrayLiteralContext *context) = 0;

    virtual std::any visitElementList(CHTLJavaScriptParser::ElementListContext *context) = 0;

    virtual std::any visitArrayElement(CHTLJavaScriptParser::ArrayElementContext *context) = 0;

    virtual std::any visitObjectLiteral(CHTLJavaScriptParser::ObjectLiteralContext *context) = 0;

    virtual std::any visitPropertyDefinitionList(CHTLJavaScriptParser::PropertyDefinitionListContext *context) = 0;

    virtual std::any visitPropertyDefinition(CHTLJavaScriptParser::PropertyDefinitionContext *context) = 0;

    virtual std::any visitFunctionExpression(CHTLJavaScriptParser::FunctionExpressionContext *context) = 0;

    virtual std::any visitGeneratorExpression(CHTLJavaScriptParser::GeneratorExpressionContext *context) = 0;

    virtual std::any visitAsyncFunctionExpression(CHTLJavaScriptParser::AsyncFunctionExpressionContext *context) = 0;

    virtual std::any visitAsyncGeneratorExpression(CHTLJavaScriptParser::AsyncGeneratorExpressionContext *context) = 0;

    virtual std::any visitClassExpression(CHTLJavaScriptParser::ClassExpressionContext *context) = 0;

    virtual std::any visitArrowFunction(CHTLJavaScriptParser::ArrowFunctionContext *context) = 0;

    virtual std::any visitAsyncArrowFunction(CHTLJavaScriptParser::AsyncArrowFunctionContext *context) = 0;

    virtual std::any visitArrowParameters(CHTLJavaScriptParser::ArrowParametersContext *context) = 0;

    virtual std::any visitArrowBody(CHTLJavaScriptParser::ArrowBodyContext *context) = 0;

    virtual std::any visitYieldExpression(CHTLJavaScriptParser::YieldExpressionContext *context) = 0;

    virtual std::any visitTemplateLiteral(CHTLJavaScriptParser::TemplateLiteralContext *context) = 0;

    virtual std::any visitTemplateSpans(CHTLJavaScriptParser::TemplateSpansContext *context) = 0;

    virtual std::any visitTemplateSpan(CHTLJavaScriptParser::TemplateSpanContext *context) = 0;

    virtual std::any visitTemplateMiddle(CHTLJavaScriptParser::TemplateMiddleContext *context) = 0;

    virtual std::any visitRegularExpressionLiteral(CHTLJavaScriptParser::RegularExpressionLiteralContext *context) = 0;

    virtual std::any visitFormalParameterList(CHTLJavaScriptParser::FormalParameterListContext *context) = 0;

    virtual std::any visitFormalParameter(CHTLJavaScriptParser::FormalParameterContext *context) = 0;

    virtual std::any visitRestParameter(CHTLJavaScriptParser::RestParameterContext *context) = 0;

    virtual std::any visitBindingElement(CHTLJavaScriptParser::BindingElementContext *context) = 0;

    virtual std::any visitBindingPattern(CHTLJavaScriptParser::BindingPatternContext *context) = 0;

    virtual std::any visitObjectBindingPattern(CHTLJavaScriptParser::ObjectBindingPatternContext *context) = 0;

    virtual std::any visitBindingPropertyList(CHTLJavaScriptParser::BindingPropertyListContext *context) = 0;

    virtual std::any visitBindingProperty(CHTLJavaScriptParser::BindingPropertyContext *context) = 0;

    virtual std::any visitArrayBindingPattern(CHTLJavaScriptParser::ArrayBindingPatternContext *context) = 0;

    virtual std::any visitBindingElementList(CHTLJavaScriptParser::BindingElementListContext *context) = 0;

    virtual std::any visitBindingElision(CHTLJavaScriptParser::BindingElisionContext *context) = 0;

    virtual std::any visitFunctionBody(CHTLJavaScriptParser::FunctionBodyContext *context) = 0;

    virtual std::any visitInitializer(CHTLJavaScriptParser::InitializerContext *context) = 0;

    virtual std::any visitBindingIdentifier(CHTLJavaScriptParser::BindingIdentifierContext *context) = 0;

    virtual std::any visitIdentifier(CHTLJavaScriptParser::IdentifierContext *context) = 0;

    virtual std::any visitIdentifierName(CHTLJavaScriptParser::IdentifierNameContext *context) = 0;

    virtual std::any visitReservedWord(CHTLJavaScriptParser::ReservedWordContext *context) = 0;

    virtual std::any visitKeyword(CHTLJavaScriptParser::KeywordContext *context) = 0;

    virtual std::any visitFutureReservedWord(CHTLJavaScriptParser::FutureReservedWordContext *context) = 0;


};

