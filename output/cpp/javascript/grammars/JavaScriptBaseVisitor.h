
// Generated from grammars/JavaScript.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "JavaScriptVisitor.h"


/**
 * This class provides an empty implementation of JavaScriptVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  JavaScriptBaseVisitor : public JavaScriptVisitor {
public:

  virtual std::any visitProgram(JavaScriptParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSourceElements(JavaScriptParser::SourceElementsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSourceElement(JavaScriptParser::SourceElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration(JavaScriptParser::DeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHoistableDeclaration(JavaScriptParser::HoistableDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGeneratorDeclaration(JavaScriptParser::GeneratorDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAsyncFunctionDeclaration(JavaScriptParser::AsyncFunctionDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAsyncGeneratorDeclaration(JavaScriptParser::AsyncGeneratorDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassDeclaration(JavaScriptParser::ClassDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassExtends(JavaScriptParser::ClassExtendsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassBody(JavaScriptParser::ClassBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassElement(JavaScriptParser::ClassElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMethodDefinition(JavaScriptParser::MethodDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFieldDefinition(JavaScriptParser::FieldDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLexicalDeclaration(JavaScriptParser::LexicalDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLetOrConst(JavaScriptParser::LetOrConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingList(JavaScriptParser::BindingListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLexicalBinding(JavaScriptParser::LexicalBindingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(JavaScriptParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockStatement(JavaScriptParser::BlockStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatementList(JavaScriptParser::StatementListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableStatement(JavaScriptParser::VariableStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclarationList(JavaScriptParser::VariableDeclarationListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclaration(JavaScriptParser::VariableDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEmptyStatement(JavaScriptParser::EmptyStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpressionStatement(JavaScriptParser::ExpressionStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfStatement(JavaScriptParser::IfStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIterationStatement(JavaScriptParser::IterationStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDoWhileStatement(JavaScriptParser::DoWhileStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWhileStatement(JavaScriptParser::WhileStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForStatement(JavaScriptParser::ForStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForInit(JavaScriptParser::ForInitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForInStatement(JavaScriptParser::ForInStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForOfStatement(JavaScriptParser::ForOfStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForInBinding(JavaScriptParser::ForInBindingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitContinueStatement(JavaScriptParser::ContinueStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBreakStatement(JavaScriptParser::BreakStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReturnStatement(JavaScriptParser::ReturnStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWithStatement(JavaScriptParser::WithStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLabelledStatement(JavaScriptParser::LabelledStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSwitchStatement(JavaScriptParser::SwitchStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCaseClause(JavaScriptParser::CaseClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDefaultClause(JavaScriptParser::DefaultClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitThrowStatement(JavaScriptParser::ThrowStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTryStatement(JavaScriptParser::TryStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCatchClause(JavaScriptParser::CatchClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCatchParameter(JavaScriptParser::CatchParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFinallyClause(JavaScriptParser::FinallyClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDebuggerStatement(JavaScriptParser::DebuggerStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportStatement(JavaScriptParser::ImportStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportClause(JavaScriptParser::ImportClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportedDefaultBinding(JavaScriptParser::ImportedDefaultBindingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamespaceImport(JavaScriptParser::NamespaceImportContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamedImports(JavaScriptParser::NamedImportsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportsList(JavaScriptParser::ImportsListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportSpecifier(JavaScriptParser::ImportSpecifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModuleSpecifier(JavaScriptParser::ModuleSpecifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportStatement(JavaScriptParser::ExportStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportClause(JavaScriptParser::ExportClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportsList(JavaScriptParser::ExportsListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportSpecifier(JavaScriptParser::ExportSpecifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(JavaScriptParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentExpression(JavaScriptParser::AssignmentExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentOperator(JavaScriptParser::AssignmentOperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConditionalExpression(JavaScriptParser::ConditionalExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCoalesceExpression(JavaScriptParser::CoalesceExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLogicalORExpression(JavaScriptParser::LogicalORExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLogicalANDExpression(JavaScriptParser::LogicalANDExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitwiseORExpression(JavaScriptParser::BitwiseORExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitwiseXORExpression(JavaScriptParser::BitwiseXORExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitwiseANDExpression(JavaScriptParser::BitwiseANDExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEqualityExpression(JavaScriptParser::EqualityExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRelationalExpression(JavaScriptParser::RelationalExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShiftExpression(JavaScriptParser::ShiftExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAdditiveExpression(JavaScriptParser::AdditiveExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMultiplicativeExpression(JavaScriptParser::MultiplicativeExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExponentiationExpression(JavaScriptParser::ExponentiationExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryExpression(JavaScriptParser::UnaryExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUpdateExpression(JavaScriptParser::UpdateExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLeftHandSideExpression(JavaScriptParser::LeftHandSideExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNewExpression(JavaScriptParser::NewExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCallExpression(JavaScriptParser::CallExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSuperCall(JavaScriptParser::SuperCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportCall(JavaScriptParser::ImportCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArguments(JavaScriptParser::ArgumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgumentList(JavaScriptParser::ArgumentListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgument(JavaScriptParser::ArgumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemberExpression(JavaScriptParser::MemberExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMetaProperty(JavaScriptParser::MetaPropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryExpression(JavaScriptParser::PrimaryExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParenthesizedExpression(JavaScriptParser::ParenthesizedExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLiteral(JavaScriptParser::LiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNullLiteral(JavaScriptParser::NullLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBooleanLiteral(JavaScriptParser::BooleanLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumericLiteral(JavaScriptParser::NumericLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStringLiteral(JavaScriptParser::StringLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayLiteral(JavaScriptParser::ArrayLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementList(JavaScriptParser::ElementListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayElement(JavaScriptParser::ArrayElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitObjectLiteral(JavaScriptParser::ObjectLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyDefinitionList(JavaScriptParser::PropertyDefinitionListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyDefinition(JavaScriptParser::PropertyDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyName(JavaScriptParser::PropertyNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionExpression(JavaScriptParser::FunctionExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGeneratorExpression(JavaScriptParser::GeneratorExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAsyncFunctionExpression(JavaScriptParser::AsyncFunctionExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAsyncGeneratorExpression(JavaScriptParser::AsyncGeneratorExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassExpression(JavaScriptParser::ClassExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrowFunction(JavaScriptParser::ArrowFunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAsyncArrowFunction(JavaScriptParser::AsyncArrowFunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrowParameters(JavaScriptParser::ArrowParametersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrowBody(JavaScriptParser::ArrowBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitYieldExpression(JavaScriptParser::YieldExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateLiteral(JavaScriptParser::TemplateLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateSpans(JavaScriptParser::TemplateSpansContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateSpan(JavaScriptParser::TemplateSpanContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateMiddle(JavaScriptParser::TemplateMiddleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRegularExpressionLiteral(JavaScriptParser::RegularExpressionLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFormalParameterList(JavaScriptParser::FormalParameterListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFormalParameter(JavaScriptParser::FormalParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRestParameter(JavaScriptParser::RestParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingElement(JavaScriptParser::BindingElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingPattern(JavaScriptParser::BindingPatternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitObjectBindingPattern(JavaScriptParser::ObjectBindingPatternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingPropertyList(JavaScriptParser::BindingPropertyListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingProperty(JavaScriptParser::BindingPropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayBindingPattern(JavaScriptParser::ArrayBindingPatternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingElementList(JavaScriptParser::BindingElementListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingElision(JavaScriptParser::BindingElisionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionBody(JavaScriptParser::FunctionBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInitializer(JavaScriptParser::InitializerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingIdentifier(JavaScriptParser::BindingIdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier(JavaScriptParser::IdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifierName(JavaScriptParser::IdentifierNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReservedWord(JavaScriptParser::ReservedWordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitKeyword(JavaScriptParser::KeywordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFutureReservedWord(JavaScriptParser::FutureReservedWordContext *ctx) override {
    return visitChildren(ctx);
  }


};

