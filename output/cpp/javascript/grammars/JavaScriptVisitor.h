
// Generated from grammars/JavaScript.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "JavaScriptParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by JavaScriptParser.
 */
class  JavaScriptVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by JavaScriptParser.
   */
    virtual std::any visitProgram(JavaScriptParser::ProgramContext *context) = 0;

    virtual std::any visitSourceElements(JavaScriptParser::SourceElementsContext *context) = 0;

    virtual std::any visitSourceElement(JavaScriptParser::SourceElementContext *context) = 0;

    virtual std::any visitDeclaration(JavaScriptParser::DeclarationContext *context) = 0;

    virtual std::any visitHoistableDeclaration(JavaScriptParser::HoistableDeclarationContext *context) = 0;

    virtual std::any visitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *context) = 0;

    virtual std::any visitGeneratorDeclaration(JavaScriptParser::GeneratorDeclarationContext *context) = 0;

    virtual std::any visitAsyncFunctionDeclaration(JavaScriptParser::AsyncFunctionDeclarationContext *context) = 0;

    virtual std::any visitAsyncGeneratorDeclaration(JavaScriptParser::AsyncGeneratorDeclarationContext *context) = 0;

    virtual std::any visitClassDeclaration(JavaScriptParser::ClassDeclarationContext *context) = 0;

    virtual std::any visitClassExtends(JavaScriptParser::ClassExtendsContext *context) = 0;

    virtual std::any visitClassBody(JavaScriptParser::ClassBodyContext *context) = 0;

    virtual std::any visitClassElement(JavaScriptParser::ClassElementContext *context) = 0;

    virtual std::any visitMethodDefinition(JavaScriptParser::MethodDefinitionContext *context) = 0;

    virtual std::any visitFieldDefinition(JavaScriptParser::FieldDefinitionContext *context) = 0;

    virtual std::any visitLexicalDeclaration(JavaScriptParser::LexicalDeclarationContext *context) = 0;

    virtual std::any visitLetOrConst(JavaScriptParser::LetOrConstContext *context) = 0;

    virtual std::any visitBindingList(JavaScriptParser::BindingListContext *context) = 0;

    virtual std::any visitLexicalBinding(JavaScriptParser::LexicalBindingContext *context) = 0;

    virtual std::any visitStatement(JavaScriptParser::StatementContext *context) = 0;

    virtual std::any visitBlockStatement(JavaScriptParser::BlockStatementContext *context) = 0;

    virtual std::any visitStatementList(JavaScriptParser::StatementListContext *context) = 0;

    virtual std::any visitVariableStatement(JavaScriptParser::VariableStatementContext *context) = 0;

    virtual std::any visitVariableDeclarationList(JavaScriptParser::VariableDeclarationListContext *context) = 0;

    virtual std::any visitVariableDeclaration(JavaScriptParser::VariableDeclarationContext *context) = 0;

    virtual std::any visitEmptyStatement(JavaScriptParser::EmptyStatementContext *context) = 0;

    virtual std::any visitExpressionStatement(JavaScriptParser::ExpressionStatementContext *context) = 0;

    virtual std::any visitIfStatement(JavaScriptParser::IfStatementContext *context) = 0;

    virtual std::any visitIterationStatement(JavaScriptParser::IterationStatementContext *context) = 0;

    virtual std::any visitDoWhileStatement(JavaScriptParser::DoWhileStatementContext *context) = 0;

    virtual std::any visitWhileStatement(JavaScriptParser::WhileStatementContext *context) = 0;

    virtual std::any visitForStatement(JavaScriptParser::ForStatementContext *context) = 0;

    virtual std::any visitForInit(JavaScriptParser::ForInitContext *context) = 0;

    virtual std::any visitForInStatement(JavaScriptParser::ForInStatementContext *context) = 0;

    virtual std::any visitForOfStatement(JavaScriptParser::ForOfStatementContext *context) = 0;

    virtual std::any visitForInBinding(JavaScriptParser::ForInBindingContext *context) = 0;

    virtual std::any visitContinueStatement(JavaScriptParser::ContinueStatementContext *context) = 0;

    virtual std::any visitBreakStatement(JavaScriptParser::BreakStatementContext *context) = 0;

    virtual std::any visitReturnStatement(JavaScriptParser::ReturnStatementContext *context) = 0;

    virtual std::any visitWithStatement(JavaScriptParser::WithStatementContext *context) = 0;

    virtual std::any visitLabelledStatement(JavaScriptParser::LabelledStatementContext *context) = 0;

    virtual std::any visitSwitchStatement(JavaScriptParser::SwitchStatementContext *context) = 0;

    virtual std::any visitCaseClause(JavaScriptParser::CaseClauseContext *context) = 0;

    virtual std::any visitDefaultClause(JavaScriptParser::DefaultClauseContext *context) = 0;

    virtual std::any visitThrowStatement(JavaScriptParser::ThrowStatementContext *context) = 0;

    virtual std::any visitTryStatement(JavaScriptParser::TryStatementContext *context) = 0;

    virtual std::any visitCatchClause(JavaScriptParser::CatchClauseContext *context) = 0;

    virtual std::any visitCatchParameter(JavaScriptParser::CatchParameterContext *context) = 0;

    virtual std::any visitFinallyClause(JavaScriptParser::FinallyClauseContext *context) = 0;

    virtual std::any visitDebuggerStatement(JavaScriptParser::DebuggerStatementContext *context) = 0;

    virtual std::any visitImportStatement(JavaScriptParser::ImportStatementContext *context) = 0;

    virtual std::any visitImportClause(JavaScriptParser::ImportClauseContext *context) = 0;

    virtual std::any visitImportedDefaultBinding(JavaScriptParser::ImportedDefaultBindingContext *context) = 0;

    virtual std::any visitNamespaceImport(JavaScriptParser::NamespaceImportContext *context) = 0;

    virtual std::any visitNamedImports(JavaScriptParser::NamedImportsContext *context) = 0;

    virtual std::any visitImportsList(JavaScriptParser::ImportsListContext *context) = 0;

    virtual std::any visitImportSpecifier(JavaScriptParser::ImportSpecifierContext *context) = 0;

    virtual std::any visitModuleSpecifier(JavaScriptParser::ModuleSpecifierContext *context) = 0;

    virtual std::any visitExportStatement(JavaScriptParser::ExportStatementContext *context) = 0;

    virtual std::any visitExportClause(JavaScriptParser::ExportClauseContext *context) = 0;

    virtual std::any visitExportsList(JavaScriptParser::ExportsListContext *context) = 0;

    virtual std::any visitExportSpecifier(JavaScriptParser::ExportSpecifierContext *context) = 0;

    virtual std::any visitExpression(JavaScriptParser::ExpressionContext *context) = 0;

    virtual std::any visitAssignmentExpression(JavaScriptParser::AssignmentExpressionContext *context) = 0;

    virtual std::any visitAssignmentOperator(JavaScriptParser::AssignmentOperatorContext *context) = 0;

    virtual std::any visitConditionalExpression(JavaScriptParser::ConditionalExpressionContext *context) = 0;

    virtual std::any visitCoalesceExpression(JavaScriptParser::CoalesceExpressionContext *context) = 0;

    virtual std::any visitLogicalORExpression(JavaScriptParser::LogicalORExpressionContext *context) = 0;

    virtual std::any visitLogicalANDExpression(JavaScriptParser::LogicalANDExpressionContext *context) = 0;

    virtual std::any visitBitwiseORExpression(JavaScriptParser::BitwiseORExpressionContext *context) = 0;

    virtual std::any visitBitwiseXORExpression(JavaScriptParser::BitwiseXORExpressionContext *context) = 0;

    virtual std::any visitBitwiseANDExpression(JavaScriptParser::BitwiseANDExpressionContext *context) = 0;

    virtual std::any visitEqualityExpression(JavaScriptParser::EqualityExpressionContext *context) = 0;

    virtual std::any visitRelationalExpression(JavaScriptParser::RelationalExpressionContext *context) = 0;

    virtual std::any visitShiftExpression(JavaScriptParser::ShiftExpressionContext *context) = 0;

    virtual std::any visitAdditiveExpression(JavaScriptParser::AdditiveExpressionContext *context) = 0;

    virtual std::any visitMultiplicativeExpression(JavaScriptParser::MultiplicativeExpressionContext *context) = 0;

    virtual std::any visitExponentiationExpression(JavaScriptParser::ExponentiationExpressionContext *context) = 0;

    virtual std::any visitUnaryExpression(JavaScriptParser::UnaryExpressionContext *context) = 0;

    virtual std::any visitUpdateExpression(JavaScriptParser::UpdateExpressionContext *context) = 0;

    virtual std::any visitLeftHandSideExpression(JavaScriptParser::LeftHandSideExpressionContext *context) = 0;

    virtual std::any visitNewExpression(JavaScriptParser::NewExpressionContext *context) = 0;

    virtual std::any visitCallExpression(JavaScriptParser::CallExpressionContext *context) = 0;

    virtual std::any visitSuperCall(JavaScriptParser::SuperCallContext *context) = 0;

    virtual std::any visitImportCall(JavaScriptParser::ImportCallContext *context) = 0;

    virtual std::any visitArguments(JavaScriptParser::ArgumentsContext *context) = 0;

    virtual std::any visitArgumentList(JavaScriptParser::ArgumentListContext *context) = 0;

    virtual std::any visitArgument(JavaScriptParser::ArgumentContext *context) = 0;

    virtual std::any visitMemberExpression(JavaScriptParser::MemberExpressionContext *context) = 0;

    virtual std::any visitMetaProperty(JavaScriptParser::MetaPropertyContext *context) = 0;

    virtual std::any visitPrimaryExpression(JavaScriptParser::PrimaryExpressionContext *context) = 0;

    virtual std::any visitParenthesizedExpression(JavaScriptParser::ParenthesizedExpressionContext *context) = 0;

    virtual std::any visitLiteral(JavaScriptParser::LiteralContext *context) = 0;

    virtual std::any visitNullLiteral(JavaScriptParser::NullLiteralContext *context) = 0;

    virtual std::any visitBooleanLiteral(JavaScriptParser::BooleanLiteralContext *context) = 0;

    virtual std::any visitNumericLiteral(JavaScriptParser::NumericLiteralContext *context) = 0;

    virtual std::any visitStringLiteral(JavaScriptParser::StringLiteralContext *context) = 0;

    virtual std::any visitArrayLiteral(JavaScriptParser::ArrayLiteralContext *context) = 0;

    virtual std::any visitElementList(JavaScriptParser::ElementListContext *context) = 0;

    virtual std::any visitArrayElement(JavaScriptParser::ArrayElementContext *context) = 0;

    virtual std::any visitObjectLiteral(JavaScriptParser::ObjectLiteralContext *context) = 0;

    virtual std::any visitPropertyDefinitionList(JavaScriptParser::PropertyDefinitionListContext *context) = 0;

    virtual std::any visitPropertyDefinition(JavaScriptParser::PropertyDefinitionContext *context) = 0;

    virtual std::any visitPropertyName(JavaScriptParser::PropertyNameContext *context) = 0;

    virtual std::any visitFunctionExpression(JavaScriptParser::FunctionExpressionContext *context) = 0;

    virtual std::any visitGeneratorExpression(JavaScriptParser::GeneratorExpressionContext *context) = 0;

    virtual std::any visitAsyncFunctionExpression(JavaScriptParser::AsyncFunctionExpressionContext *context) = 0;

    virtual std::any visitAsyncGeneratorExpression(JavaScriptParser::AsyncGeneratorExpressionContext *context) = 0;

    virtual std::any visitClassExpression(JavaScriptParser::ClassExpressionContext *context) = 0;

    virtual std::any visitArrowFunction(JavaScriptParser::ArrowFunctionContext *context) = 0;

    virtual std::any visitAsyncArrowFunction(JavaScriptParser::AsyncArrowFunctionContext *context) = 0;

    virtual std::any visitArrowParameters(JavaScriptParser::ArrowParametersContext *context) = 0;

    virtual std::any visitArrowBody(JavaScriptParser::ArrowBodyContext *context) = 0;

    virtual std::any visitYieldExpression(JavaScriptParser::YieldExpressionContext *context) = 0;

    virtual std::any visitTemplateLiteral(JavaScriptParser::TemplateLiteralContext *context) = 0;

    virtual std::any visitTemplateSpans(JavaScriptParser::TemplateSpansContext *context) = 0;

    virtual std::any visitTemplateSpan(JavaScriptParser::TemplateSpanContext *context) = 0;

    virtual std::any visitTemplateMiddle(JavaScriptParser::TemplateMiddleContext *context) = 0;

    virtual std::any visitRegularExpressionLiteral(JavaScriptParser::RegularExpressionLiteralContext *context) = 0;

    virtual std::any visitFormalParameterList(JavaScriptParser::FormalParameterListContext *context) = 0;

    virtual std::any visitFormalParameter(JavaScriptParser::FormalParameterContext *context) = 0;

    virtual std::any visitRestParameter(JavaScriptParser::RestParameterContext *context) = 0;

    virtual std::any visitBindingElement(JavaScriptParser::BindingElementContext *context) = 0;

    virtual std::any visitBindingPattern(JavaScriptParser::BindingPatternContext *context) = 0;

    virtual std::any visitObjectBindingPattern(JavaScriptParser::ObjectBindingPatternContext *context) = 0;

    virtual std::any visitBindingPropertyList(JavaScriptParser::BindingPropertyListContext *context) = 0;

    virtual std::any visitBindingProperty(JavaScriptParser::BindingPropertyContext *context) = 0;

    virtual std::any visitArrayBindingPattern(JavaScriptParser::ArrayBindingPatternContext *context) = 0;

    virtual std::any visitBindingElementList(JavaScriptParser::BindingElementListContext *context) = 0;

    virtual std::any visitBindingElision(JavaScriptParser::BindingElisionContext *context) = 0;

    virtual std::any visitFunctionBody(JavaScriptParser::FunctionBodyContext *context) = 0;

    virtual std::any visitInitializer(JavaScriptParser::InitializerContext *context) = 0;

    virtual std::any visitBindingIdentifier(JavaScriptParser::BindingIdentifierContext *context) = 0;

    virtual std::any visitIdentifier(JavaScriptParser::IdentifierContext *context) = 0;

    virtual std::any visitIdentifierName(JavaScriptParser::IdentifierNameContext *context) = 0;

    virtual std::any visitReservedWord(JavaScriptParser::ReservedWordContext *context) = 0;

    virtual std::any visitKeyword(JavaScriptParser::KeywordContext *context) = 0;

    virtual std::any visitFutureReservedWord(JavaScriptParser::FutureReservedWordContext *context) = 0;


};

