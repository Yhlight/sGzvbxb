
// Generated from grammars/CHTL.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by CHTLParser.
 */
class  CHTLVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by CHTLParser.
   */
    virtual std::any visitProgram(CHTLParser::ProgramContext *context) = 0;

    virtual std::any visitTopLevelStatement(CHTLParser::TopLevelStatementContext *context) = 0;

    virtual std::any visitConfigurationBlock(CHTLParser::ConfigurationBlockContext *context) = 0;

    virtual std::any visitConfigurationItem(CHTLParser::ConfigurationItemContext *context) = 0;

    virtual std::any visitConfigKey(CHTLParser::ConfigKeyContext *context) = 0;

    virtual std::any visitConfigValue(CHTLParser::ConfigValueContext *context) = 0;

    virtual std::any visitConfigValueArray(CHTLParser::ConfigValueArrayContext *context) = 0;

    virtual std::any visitConfigArrayItem(CHTLParser::ConfigArrayItemContext *context) = 0;

    virtual std::any visitNameConfigItem(CHTLParser::NameConfigItemContext *context) = 0;

    virtual std::any visitNamespaceDeclaration(CHTLParser::NamespaceDeclarationContext *context) = 0;

    virtual std::any visitNamespaceBody(CHTLParser::NamespaceBodyContext *context) = 0;

    virtual std::any visitNamespaceContent(CHTLParser::NamespaceContentContext *context) = 0;

    virtual std::any visitImportStatement(CHTLParser::ImportStatementContext *context) = 0;

    virtual std::any visitImportType(CHTLParser::ImportTypeContext *context) = 0;

    virtual std::any visitImportPath(CHTLParser::ImportPathContext *context) = 0;

    virtual std::any visitDottedPath(CHTLParser::DottedPathContext *context) = 0;

    virtual std::any visitTemplateDeclaration(CHTLParser::TemplateDeclarationContext *context) = 0;

    virtual std::any visitTemplateType(CHTLParser::TemplateTypeContext *context) = 0;

    virtual std::any visitTemplateBody(CHTLParser::TemplateBodyContext *context) = 0;

    virtual std::any visitTemplateContent(CHTLParser::TemplateContentContext *context) = 0;

    virtual std::any visitTemplateInheritance(CHTLParser::TemplateInheritanceContext *context) = 0;

    virtual std::any visitCustomDeclaration(CHTLParser::CustomDeclarationContext *context) = 0;

    virtual std::any visitCustomType(CHTLParser::CustomTypeContext *context) = 0;

    virtual std::any visitCustomBody(CHTLParser::CustomBodyContext *context) = 0;

    virtual std::any visitCustomContent(CHTLParser::CustomContentContext *context) = 0;

    virtual std::any visitCustomInheritance(CHTLParser::CustomInheritanceContext *context) = 0;

    virtual std::any visitCustomSpecialization(CHTLParser::CustomSpecializationContext *context) = 0;

    virtual std::any visitSpecializationContent(CHTLParser::SpecializationContentContext *context) = 0;

    virtual std::any visitDeleteStatement(CHTLParser::DeleteStatementContext *context) = 0;

    virtual std::any visitDeleteTarget(CHTLParser::DeleteTargetContext *context) = 0;

    virtual std::any visitInsertStatement(CHTLParser::InsertStatementContext *context) = 0;

    virtual std::any visitInsertPosition(CHTLParser::InsertPositionContext *context) = 0;

    virtual std::any visitElementSelector(CHTLParser::ElementSelectorContext *context) = 0;

    virtual std::any visitInsertContent(CHTLParser::InsertContentContext *context) = 0;

    virtual std::any visitOriginBlock(CHTLParser::OriginBlockContext *context) = 0;

    virtual std::any visitOriginType(CHTLParser::OriginTypeContext *context) = 0;

    virtual std::any visitOriginContent(CHTLParser::OriginContentContext *context) = 0;

    virtual std::any visitHtmlElement(CHTLParser::HtmlElementContext *context) = 0;

    virtual std::any visitGlobalStyleElement(CHTLParser::GlobalStyleElementContext *context) = 0;

    virtual std::any visitGlobalStyleContent(CHTLParser::GlobalStyleContentContext *context) = 0;

    virtual std::any visitElementName(CHTLParser::ElementNameContext *context) = 0;

    virtual std::any visitElementIndexAccess(CHTLParser::ElementIndexAccessContext *context) = 0;

    virtual std::any visitElementBody(CHTLParser::ElementBodyContext *context) = 0;

    virtual std::any visitElementContent(CHTLParser::ElementContentContext *context) = 0;

    virtual std::any visitAttribute(CHTLParser::AttributeContext *context) = 0;

    virtual std::any visitAttributeName(CHTLParser::AttributeNameContext *context) = 0;

    virtual std::any visitAttributeValue(CHTLParser::AttributeValueContext *context) = 0;

    virtual std::any visitTextContent(CHTLParser::TextContentContext *context) = 0;

    virtual std::any visitStyleBlock(CHTLParser::StyleBlockContext *context) = 0;

    virtual std::any visitStyleContent(CHTLParser::StyleContentContext *context) = 0;

    virtual std::any visitStyleProperty(CHTLParser::StylePropertyContext *context) = 0;

    virtual std::any visitPropertyName(CHTLParser::PropertyNameContext *context) = 0;

    virtual std::any visitPropertyValue(CHTLParser::PropertyValueContext *context) = 0;

    virtual std::any visitCssSelector(CHTLParser::CssSelectorContext *context) = 0;

    virtual std::any visitPseudoClass(CHTLParser::PseudoClassContext *context) = 0;

    virtual std::any visitPseudoElement(CHTLParser::PseudoElementContext *context) = 0;

    virtual std::any visitCssFunction(CHTLParser::CssFunctionContext *context) = 0;

    virtual std::any visitCssArguments(CHTLParser::CssArgumentsContext *context) = 0;

    virtual std::any visitCssArgument(CHTLParser::CssArgumentContext *context) = 0;

    virtual std::any visitScriptBlock(CHTLParser::ScriptBlockContext *context) = 0;

    virtual std::any visitScriptContent(CHTLParser::ScriptContentContext *context) = 0;

    virtual std::any visitElementUsage(CHTLParser::ElementUsageContext *context) = 0;

    virtual std::any visitElementSpecialization(CHTLParser::ElementSpecializationContext *context) = 0;

    virtual std::any visitElementSpecializationContent(CHTLParser::ElementSpecializationContentContext *context) = 0;

    virtual std::any visitStyleUsage(CHTLParser::StyleUsageContext *context) = 0;

    virtual std::any visitStyleSpecialization(CHTLParser::StyleSpecializationContext *context) = 0;

    virtual std::any visitVariableDefinition(CHTLParser::VariableDefinitionContext *context) = 0;

    virtual std::any visitVariableUsage(CHTLParser::VariableUsageContext *context) = 0;

    virtual std::any visitVariableArguments(CHTLParser::VariableArgumentsContext *context) = 0;

    virtual std::any visitVariableArgument(CHTLParser::VariableArgumentContext *context) = 0;

    virtual std::any visitVariableAssignment(CHTLParser::VariableAssignmentContext *context) = 0;

    virtual std::any visitNamespacePath(CHTLParser::NamespacePathContext *context) = 0;

    virtual std::any visitExceptConstraint(CHTLParser::ExceptConstraintContext *context) = 0;

    virtual std::any visitExceptTarget(CHTLParser::ExceptTargetContext *context) = 0;

    virtual std::any visitComment(CHTLParser::CommentContext *context) = 0;


};

