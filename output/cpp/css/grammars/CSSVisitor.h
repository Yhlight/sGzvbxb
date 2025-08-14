
// Generated from grammars/CSS.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CSSParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by CSSParser.
 */
class  CSSVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by CSSParser.
   */
    virtual std::any visitStylesheet(CSSParser::StylesheetContext *context) = 0;

    virtual std::any visitStatement(CSSParser::StatementContext *context) = 0;

    virtual std::any visitRuleset(CSSParser::RulesetContext *context) = 0;

    virtual std::any visitSelectorList(CSSParser::SelectorListContext *context) = 0;

    virtual std::any visitSelector(CSSParser::SelectorContext *context) = 0;

    virtual std::any visitCombinator(CSSParser::CombinatorContext *context) = 0;

    virtual std::any visitSimpleSelector(CSSParser::SimpleSelectorContext *context) = 0;

    virtual std::any visitTypeSelector(CSSParser::TypeSelectorContext *context) = 0;

    virtual std::any visitUniversalSelector(CSSParser::UniversalSelectorContext *context) = 0;

    virtual std::any visitIdSelector(CSSParser::IdSelectorContext *context) = 0;

    virtual std::any visitClassSelector(CSSParser::ClassSelectorContext *context) = 0;

    virtual std::any visitAttributeSelector(CSSParser::AttributeSelectorContext *context) = 0;

    virtual std::any visitAttributeOperator(CSSParser::AttributeOperatorContext *context) = 0;

    virtual std::any visitPseudoSelector(CSSParser::PseudoSelectorContext *context) = 0;

    virtual std::any visitPseudoClass(CSSParser::PseudoClassContext *context) = 0;

    virtual std::any visitPseudoClassArgument(CSSParser::PseudoClassArgumentContext *context) = 0;

    virtual std::any visitPseudoElement(CSSParser::PseudoElementContext *context) = 0;

    virtual std::any visitDeclaration(CSSParser::DeclarationContext *context) = 0;

    virtual std::any visitProperty(CSSParser::PropertyContext *context) = 0;

    virtual std::any visitValue(CSSParser::ValueContext *context) = 0;

    virtual std::any visitOperator(CSSParser::OperatorContext *context) = 0;

    virtual std::any visitTerm(CSSParser::TermContext *context) = 0;

    virtual std::any visitUnaryOperator(CSSParser::UnaryOperatorContext *context) = 0;

    virtual std::any visitNumeric(CSSParser::NumericContext *context) = 0;

    virtual std::any visitPercentage(CSSParser::PercentageContext *context) = 0;

    virtual std::any visitLength(CSSParser::LengthContext *context) = 0;

    virtual std::any visitAngle(CSSParser::AngleContext *context) = 0;

    virtual std::any visitTime(CSSParser::TimeContext *context) = 0;

    virtual std::any visitFrequency(CSSParser::FrequencyContext *context) = 0;

    virtual std::any visitResolution(CSSParser::ResolutionContext *context) = 0;

    virtual std::any visitHexcolor(CSSParser::HexcolorContext *context) = 0;

    virtual std::any visitFunction(CSSParser::FunctionContext *context) = 0;

    virtual std::any visitFunctionArguments(CSSParser::FunctionArgumentsContext *context) = 0;

    virtual std::any visitFunctionArgument(CSSParser::FunctionArgumentContext *context) = 0;

    virtual std::any visitCalc(CSSParser::CalcContext *context) = 0;

    virtual std::any visitCalcExpression(CSSParser::CalcExpressionContext *context) = 0;

    virtual std::any visitCalcTerm(CSSParser::CalcTermContext *context) = 0;

    virtual std::any visitCalcFactor(CSSParser::CalcFactorContext *context) = 0;

    virtual std::any visitAtRule(CSSParser::AtRuleContext *context) = 0;

    virtual std::any visitAtImport(CSSParser::AtImportContext *context) = 0;

    virtual std::any visitAtMedia(CSSParser::AtMediaContext *context) = 0;

    virtual std::any visitMediaQueryList(CSSParser::MediaQueryListContext *context) = 0;

    virtual std::any visitMediaQuery(CSSParser::MediaQueryContext *context) = 0;

    virtual std::any visitMediaType(CSSParser::MediaTypeContext *context) = 0;

    virtual std::any visitMediaFeature(CSSParser::MediaFeatureContext *context) = 0;

    virtual std::any visitAtKeyframes(CSSParser::AtKeyframesContext *context) = 0;

    virtual std::any visitKeyframeBlock(CSSParser::KeyframeBlockContext *context) = 0;

    virtual std::any visitKeyframeSelector(CSSParser::KeyframeSelectorContext *context) = 0;

    virtual std::any visitAtFontFace(CSSParser::AtFontFaceContext *context) = 0;

    virtual std::any visitAtSupports(CSSParser::AtSupportsContext *context) = 0;

    virtual std::any visitSupportsCondition(CSSParser::SupportsConditionContext *context) = 0;

    virtual std::any visitSupportsNegation(CSSParser::SupportsNegationContext *context) = 0;

    virtual std::any visitSupportsConjunction(CSSParser::SupportsConjunctionContext *context) = 0;

    virtual std::any visitSupportsDisjunction(CSSParser::SupportsDisjunctionContext *context) = 0;

    virtual std::any visitSupportsInParens(CSSParser::SupportsInParensContext *context) = 0;

    virtual std::any visitSupportsFeature(CSSParser::SupportsFeatureContext *context) = 0;

    virtual std::any visitAtPage(CSSParser::AtPageContext *context) = 0;

    virtual std::any visitPseudoPage(CSSParser::PseudoPageContext *context) = 0;

    virtual std::any visitAtNamespace(CSSParser::AtNamespaceContext *context) = 0;

    virtual std::any visitAtCharset(CSSParser::AtCharsetContext *context) = 0;

    virtual std::any visitAtViewport(CSSParser::AtViewportContext *context) = 0;

    virtual std::any visitAtCounterStyle(CSSParser::AtCounterStyleContext *context) = 0;

    virtual std::any visitAtFontFeatureValues(CSSParser::AtFontFeatureValuesContext *context) = 0;

    virtual std::any visitFontFamilyName(CSSParser::FontFamilyNameContext *context) = 0;

    virtual std::any visitFontFeatureValueBlock(CSSParser::FontFeatureValueBlockContext *context) = 0;

    virtual std::any visitAtProperty(CSSParser::AtPropertyContext *context) = 0;

    virtual std::any visitExpression(CSSParser::ExpressionContext *context) = 0;

    virtual std::any visitComment(CSSParser::CommentContext *context) = 0;


};

