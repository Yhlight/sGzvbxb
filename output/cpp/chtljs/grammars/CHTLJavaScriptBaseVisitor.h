
// Generated from grammars/CHTLJavaScript.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLJavaScriptVisitor.h"


/**
 * This class provides an empty implementation of CHTLJavaScriptVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  CHTLJavaScriptBaseVisitor : public CHTLJavaScriptVisitor {
public:

  virtual std::any visitPrimaryExpression(CHTLJavaScriptParser::PrimaryExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChtlSelector(CHTLJavaScriptParser::ChtlSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectorExpression(CHTLJavaScriptParser::SelectorExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectorIdentifier(CHTLJavaScriptParser::SelectorIdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectorIndex(CHTLJavaScriptParser::SelectorIndexContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComplexSelector(CHTLJavaScriptParser::ComplexSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectorPart(CHTLJavaScriptParser::SelectorPartContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMemberExpression(CHTLJavaScriptParser::MemberExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCallExpression(CHTLJavaScriptParser::CallExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnimateCall(CHTLJavaScriptParser::AnimateCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitListenConfig(CHTLJavaScriptParser::ListenConfigContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEventHandlerList(CHTLJavaScriptParser::EventHandlerListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEventHandler(CHTLJavaScriptParser::EventHandlerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDelegateConfig(CHTLJavaScriptParser::DelegateConfigContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDelegateProperty(CHTLJavaScriptParser::DelegatePropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectorArray(CHTLJavaScriptParser::SelectorArrayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnimateConfig(CHTLJavaScriptParser::AnimateConfigContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnimateProperty(CHTLJavaScriptParser::AnimatePropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCssBlock(CHTLJavaScriptParser::CssBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCssProperty(CHTLJavaScriptParser::CssPropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyName(CHTLJavaScriptParser::PropertyNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyValue(CHTLJavaScriptParser::PropertyValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCssFunction(CHTLJavaScriptParser::CssFunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCssFunctionArguments(CHTLJavaScriptParser::CssFunctionArgumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWhenArray(CHTLJavaScriptParser::WhenArrayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWhenItem(CHTLJavaScriptParser::WhenItemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWhenProperty(CHTLJavaScriptParser::WhenPropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLiteral(CHTLJavaScriptParser::LiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProgram(CHTLJavaScriptParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSourceElements(CHTLJavaScriptParser::SourceElementsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSourceElement(CHTLJavaScriptParser::SourceElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration(CHTLJavaScriptParser::DeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHoistableDeclaration(CHTLJavaScriptParser::HoistableDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionDeclaration(CHTLJavaScriptParser::FunctionDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGeneratorDeclaration(CHTLJavaScriptParser::GeneratorDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAsyncFunctionDeclaration(CHTLJavaScriptParser::AsyncFunctionDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAsyncGeneratorDeclaration(CHTLJavaScriptParser::AsyncGeneratorDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassDeclaration(CHTLJavaScriptParser::ClassDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassExtends(CHTLJavaScriptParser::ClassExtendsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassBody(CHTLJavaScriptParser::ClassBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassElement(CHTLJavaScriptParser::ClassElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMethodDefinition(CHTLJavaScriptParser::MethodDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFieldDefinition(CHTLJavaScriptParser::FieldDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLexicalDeclaration(CHTLJavaScriptParser::LexicalDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLetOrConst(CHTLJavaScriptParser::LetOrConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingList(CHTLJavaScriptParser::BindingListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLexicalBinding(CHTLJavaScriptParser::LexicalBindingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(CHTLJavaScriptParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockStatement(CHTLJavaScriptParser::BlockStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatementList(CHTLJavaScriptParser::StatementListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableStatement(CHTLJavaScriptParser::VariableStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclarationList(CHTLJavaScriptParser::VariableDeclarationListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclaration(CHTLJavaScriptParser::VariableDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEmptyStatement(CHTLJavaScriptParser::EmptyStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpressionStatement(CHTLJavaScriptParser::ExpressionStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfStatement(CHTLJavaScriptParser::IfStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIterationStatement(CHTLJavaScriptParser::IterationStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDoWhileStatement(CHTLJavaScriptParser::DoWhileStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWhileStatement(CHTLJavaScriptParser::WhileStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForStatement(CHTLJavaScriptParser::ForStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForInit(CHTLJavaScriptParser::ForInitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForInStatement(CHTLJavaScriptParser::ForInStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForOfStatement(CHTLJavaScriptParser::ForOfStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForInBinding(CHTLJavaScriptParser::ForInBindingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitContinueStatement(CHTLJavaScriptParser::ContinueStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBreakStatement(CHTLJavaScriptParser::BreakStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReturnStatement(CHTLJavaScriptParser::ReturnStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWithStatement(CHTLJavaScriptParser::WithStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLabelledStatement(CHTLJavaScriptParser::LabelledStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSwitchStatement(CHTLJavaScriptParser::SwitchStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCaseClause(CHTLJavaScriptParser::CaseClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDefaultClause(CHTLJavaScriptParser::DefaultClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitThrowStatement(CHTLJavaScriptParser::ThrowStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTryStatement(CHTLJavaScriptParser::TryStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCatchClause(CHTLJavaScriptParser::CatchClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCatchParameter(CHTLJavaScriptParser::CatchParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFinallyClause(CHTLJavaScriptParser::FinallyClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDebuggerStatement(CHTLJavaScriptParser::DebuggerStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportStatement(CHTLJavaScriptParser::ImportStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportClause(CHTLJavaScriptParser::ImportClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportedDefaultBinding(CHTLJavaScriptParser::ImportedDefaultBindingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamespaceImport(CHTLJavaScriptParser::NamespaceImportContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamedImports(CHTLJavaScriptParser::NamedImportsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportsList(CHTLJavaScriptParser::ImportsListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportSpecifier(CHTLJavaScriptParser::ImportSpecifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModuleSpecifier(CHTLJavaScriptParser::ModuleSpecifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportStatement(CHTLJavaScriptParser::ExportStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportClause(CHTLJavaScriptParser::ExportClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportsList(CHTLJavaScriptParser::ExportsListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExportSpecifier(CHTLJavaScriptParser::ExportSpecifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(CHTLJavaScriptParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentExpression(CHTLJavaScriptParser::AssignmentExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentOperator(CHTLJavaScriptParser::AssignmentOperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConditionalExpression(CHTLJavaScriptParser::ConditionalExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCoalesceExpression(CHTLJavaScriptParser::CoalesceExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLogicalORExpression(CHTLJavaScriptParser::LogicalORExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLogicalANDExpression(CHTLJavaScriptParser::LogicalANDExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitwiseORExpression(CHTLJavaScriptParser::BitwiseORExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitwiseXORExpression(CHTLJavaScriptParser::BitwiseXORExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBitwiseANDExpression(CHTLJavaScriptParser::BitwiseANDExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEqualityExpression(CHTLJavaScriptParser::EqualityExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRelationalExpression(CHTLJavaScriptParser::RelationalExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShiftExpression(CHTLJavaScriptParser::ShiftExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAdditiveExpression(CHTLJavaScriptParser::AdditiveExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMultiplicativeExpression(CHTLJavaScriptParser::MultiplicativeExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExponentiationExpression(CHTLJavaScriptParser::ExponentiationExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryExpression(CHTLJavaScriptParser::UnaryExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUpdateExpression(CHTLJavaScriptParser::UpdateExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLeftHandSideExpression(CHTLJavaScriptParser::LeftHandSideExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNewExpression(CHTLJavaScriptParser::NewExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSuperCall(CHTLJavaScriptParser::SuperCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportCall(CHTLJavaScriptParser::ImportCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArguments(CHTLJavaScriptParser::ArgumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgumentList(CHTLJavaScriptParser::ArgumentListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgument(CHTLJavaScriptParser::ArgumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMetaProperty(CHTLJavaScriptParser::MetaPropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParenthesizedExpression(CHTLJavaScriptParser::ParenthesizedExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNullLiteral(CHTLJavaScriptParser::NullLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBooleanLiteral(CHTLJavaScriptParser::BooleanLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumericLiteral(CHTLJavaScriptParser::NumericLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStringLiteral(CHTLJavaScriptParser::StringLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayLiteral(CHTLJavaScriptParser::ArrayLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementList(CHTLJavaScriptParser::ElementListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayElement(CHTLJavaScriptParser::ArrayElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitObjectLiteral(CHTLJavaScriptParser::ObjectLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyDefinitionList(CHTLJavaScriptParser::PropertyDefinitionListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyDefinition(CHTLJavaScriptParser::PropertyDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionExpression(CHTLJavaScriptParser::FunctionExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGeneratorExpression(CHTLJavaScriptParser::GeneratorExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAsyncFunctionExpression(CHTLJavaScriptParser::AsyncFunctionExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAsyncGeneratorExpression(CHTLJavaScriptParser::AsyncGeneratorExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassExpression(CHTLJavaScriptParser::ClassExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrowFunction(CHTLJavaScriptParser::ArrowFunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAsyncArrowFunction(CHTLJavaScriptParser::AsyncArrowFunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrowParameters(CHTLJavaScriptParser::ArrowParametersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrowBody(CHTLJavaScriptParser::ArrowBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitYieldExpression(CHTLJavaScriptParser::YieldExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateLiteral(CHTLJavaScriptParser::TemplateLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateSpans(CHTLJavaScriptParser::TemplateSpansContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateSpan(CHTLJavaScriptParser::TemplateSpanContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateMiddle(CHTLJavaScriptParser::TemplateMiddleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRegularExpressionLiteral(CHTLJavaScriptParser::RegularExpressionLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFormalParameterList(CHTLJavaScriptParser::FormalParameterListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFormalParameter(CHTLJavaScriptParser::FormalParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRestParameter(CHTLJavaScriptParser::RestParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingElement(CHTLJavaScriptParser::BindingElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingPattern(CHTLJavaScriptParser::BindingPatternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitObjectBindingPattern(CHTLJavaScriptParser::ObjectBindingPatternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingPropertyList(CHTLJavaScriptParser::BindingPropertyListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingProperty(CHTLJavaScriptParser::BindingPropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayBindingPattern(CHTLJavaScriptParser::ArrayBindingPatternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingElementList(CHTLJavaScriptParser::BindingElementListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingElision(CHTLJavaScriptParser::BindingElisionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionBody(CHTLJavaScriptParser::FunctionBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInitializer(CHTLJavaScriptParser::InitializerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBindingIdentifier(CHTLJavaScriptParser::BindingIdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier(CHTLJavaScriptParser::IdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifierName(CHTLJavaScriptParser::IdentifierNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReservedWord(CHTLJavaScriptParser::ReservedWordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitKeyword(CHTLJavaScriptParser::KeywordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFutureReservedWord(CHTLJavaScriptParser::FutureReservedWordContext *ctx) override {
    return visitChildren(ctx);
  }


};

