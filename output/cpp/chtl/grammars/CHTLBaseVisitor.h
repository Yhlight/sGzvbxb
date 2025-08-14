
// Generated from grammars/CHTL.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLVisitor.h"


/**
 * This class provides an empty implementation of CHTLVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  CHTLBaseVisitor : public CHTLVisitor {
public:

  virtual std::any visitProgram(CHTLParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTopLevelStatement(CHTLParser::TopLevelStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConfigurationBlock(CHTLParser::ConfigurationBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConfigurationItem(CHTLParser::ConfigurationItemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConfigKey(CHTLParser::ConfigKeyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConfigValue(CHTLParser::ConfigValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConfigValueArray(CHTLParser::ConfigValueArrayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConfigArrayItem(CHTLParser::ConfigArrayItemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNameConfigItem(CHTLParser::NameConfigItemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamespaceDeclaration(CHTLParser::NamespaceDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamespaceBody(CHTLParser::NamespaceBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamespaceContent(CHTLParser::NamespaceContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportStatement(CHTLParser::ImportStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportType(CHTLParser::ImportTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportPath(CHTLParser::ImportPathContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDottedPath(CHTLParser::DottedPathContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateDeclaration(CHTLParser::TemplateDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateType(CHTLParser::TemplateTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateBody(CHTLParser::TemplateBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateContent(CHTLParser::TemplateContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateInheritance(CHTLParser::TemplateInheritanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCustomDeclaration(CHTLParser::CustomDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCustomType(CHTLParser::CustomTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCustomBody(CHTLParser::CustomBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCustomContent(CHTLParser::CustomContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCustomInheritance(CHTLParser::CustomInheritanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCustomSpecialization(CHTLParser::CustomSpecializationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpecializationContent(CHTLParser::SpecializationContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeleteStatement(CHTLParser::DeleteStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeleteTarget(CHTLParser::DeleteTargetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInsertStatement(CHTLParser::InsertStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInsertPosition(CHTLParser::InsertPositionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementSelector(CHTLParser::ElementSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInsertContent(CHTLParser::InsertContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOriginBlock(CHTLParser::OriginBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOriginType(CHTLParser::OriginTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOriginContent(CHTLParser::OriginContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHtmlElement(CHTLParser::HtmlElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGlobalStyleElement(CHTLParser::GlobalStyleElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGlobalStyleContent(CHTLParser::GlobalStyleContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementName(CHTLParser::ElementNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementIndexAccess(CHTLParser::ElementIndexAccessContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementBody(CHTLParser::ElementBodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementContent(CHTLParser::ElementContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttribute(CHTLParser::AttributeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttributeName(CHTLParser::AttributeNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttributeValue(CHTLParser::AttributeValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTextContent(CHTLParser::TextContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStyleBlock(CHTLParser::StyleBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStyleContent(CHTLParser::StyleContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStyleProperty(CHTLParser::StylePropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyName(CHTLParser::PropertyNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyValue(CHTLParser::PropertyValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCssSelector(CHTLParser::CssSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPseudoClass(CHTLParser::PseudoClassContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPseudoElement(CHTLParser::PseudoElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCssFunction(CHTLParser::CssFunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCssArguments(CHTLParser::CssArgumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCssArgument(CHTLParser::CssArgumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitScriptBlock(CHTLParser::ScriptBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitScriptContent(CHTLParser::ScriptContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementUsage(CHTLParser::ElementUsageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementSpecialization(CHTLParser::ElementSpecializationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementSpecializationContent(CHTLParser::ElementSpecializationContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStyleUsage(CHTLParser::StyleUsageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStyleSpecialization(CHTLParser::StyleSpecializationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDefinition(CHTLParser::VariableDefinitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableUsage(CHTLParser::VariableUsageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableArguments(CHTLParser::VariableArgumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableArgument(CHTLParser::VariableArgumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableAssignment(CHTLParser::VariableAssignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamespacePath(CHTLParser::NamespacePathContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExceptConstraint(CHTLParser::ExceptConstraintContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExceptTarget(CHTLParser::ExceptTargetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComment(CHTLParser::CommentContext *ctx) override {
    return visitChildren(ctx);
  }


};

