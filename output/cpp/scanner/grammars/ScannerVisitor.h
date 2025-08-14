
// Generated from grammars/Scanner.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "ScannerParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by ScannerParser.
 */
class  ScannerVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by ScannerParser.
   */
    virtual std::any visitDocument(ScannerParser::DocumentContext *context) = 0;

    virtual std::any visitCodeBlock(ScannerParser::CodeBlockContext *context) = 0;

    virtual std::any visitChtlBlock(ScannerParser::ChtlBlockContext *context) = 0;

    virtual std::any visitChtlElement(ScannerParser::ChtlElementContext *context) = 0;

    virtual std::any visitElementStart(ScannerParser::ElementStartContext *context) = 0;

    virtual std::any visitElementEnd(ScannerParser::ElementEndContext *context) = 0;

    virtual std::any visitElementContent(ScannerParser::ElementContentContext *context) = 0;

    virtual std::any visitChtlAttribute(ScannerParser::ChtlAttributeContext *context) = 0;

    virtual std::any visitChtlDeclaration(ScannerParser::ChtlDeclarationContext *context) = 0;

    virtual std::any visitChtlStatement(ScannerParser::ChtlStatementContext *context) = 0;

    virtual std::any visitTextBlock(ScannerParser::TextBlockContext *context) = 0;

    virtual std::any visitTextContent(ScannerParser::TextContentContext *context) = 0;


};

