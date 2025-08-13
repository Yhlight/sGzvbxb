
// Generated from grammars/CSS.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CSSVisitor.h"


/**
 * This class provides an empty implementation of CSSVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  CSSBaseVisitor : public CSSVisitor {
public:

  virtual std::any visitStylesheet(CSSParser::StylesheetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(CSSParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRuleset(CSSParser::RulesetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectorList(CSSParser::SelectorListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelector(CSSParser::SelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCombinator(CSSParser::CombinatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSimpleSelector(CSSParser::SimpleSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeSelector(CSSParser::TypeSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUniversalSelector(CSSParser::UniversalSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdSelector(CSSParser::IdSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClassSelector(CSSParser::ClassSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttributeSelector(CSSParser::AttributeSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttributeOperator(CSSParser::AttributeOperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPseudoSelector(CSSParser::PseudoSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPseudoClass(CSSParser::PseudoClassContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPseudoClassArgument(CSSParser::PseudoClassArgumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPseudoElement(CSSParser::PseudoElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration(CSSParser::DeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty(CSSParser::PropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValue(CSSParser::ValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator(CSSParser::OperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTerm(CSSParser::TermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryOperator(CSSParser::UnaryOperatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumeric(CSSParser::NumericContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPercentage(CSSParser::PercentageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLength(CSSParser::LengthContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAngle(CSSParser::AngleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTime(CSSParser::TimeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFrequency(CSSParser::FrequencyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitResolution(CSSParser::ResolutionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHexcolor(CSSParser::HexcolorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction(CSSParser::FunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionArguments(CSSParser::FunctionArgumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionArgument(CSSParser::FunctionArgumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCalc(CSSParser::CalcContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCalcExpression(CSSParser::CalcExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCalcTerm(CSSParser::CalcTermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCalcFactor(CSSParser::CalcFactorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtRule(CSSParser::AtRuleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtImport(CSSParser::AtImportContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtMedia(CSSParser::AtMediaContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMediaQueryList(CSSParser::MediaQueryListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMediaQuery(CSSParser::MediaQueryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMediaType(CSSParser::MediaTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMediaFeature(CSSParser::MediaFeatureContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtKeyframes(CSSParser::AtKeyframesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitKeyframeBlock(CSSParser::KeyframeBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitKeyframeSelector(CSSParser::KeyframeSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtFontFace(CSSParser::AtFontFaceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtSupports(CSSParser::AtSupportsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupportsCondition(CSSParser::SupportsConditionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupportsNegation(CSSParser::SupportsNegationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupportsConjunction(CSSParser::SupportsConjunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupportsDisjunction(CSSParser::SupportsDisjunctionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupportsInParens(CSSParser::SupportsInParensContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSupportsFeature(CSSParser::SupportsFeatureContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtPage(CSSParser::AtPageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPseudoPage(CSSParser::PseudoPageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtNamespace(CSSParser::AtNamespaceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtCharset(CSSParser::AtCharsetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtViewport(CSSParser::AtViewportContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtCounterStyle(CSSParser::AtCounterStyleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtFontFeatureValues(CSSParser::AtFontFeatureValuesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFontFamilyName(CSSParser::FontFamilyNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFontFeatureValueBlock(CSSParser::FontFeatureValueBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAtProperty(CSSParser::AtPropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(CSSParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComment(CSSParser::CommentContext *ctx) override {
    return visitChildren(ctx);
  }


};

