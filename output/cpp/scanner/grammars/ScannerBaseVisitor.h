
// Generated from grammars/Scanner.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "ScannerVisitor.h"


/**
 * This class provides an empty implementation of ScannerVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  ScannerBaseVisitor : public ScannerVisitor {
public:

  virtual std::any visitDocument(ScannerParser::DocumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCodeBlock(ScannerParser::CodeBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChtlBlock(ScannerParser::ChtlBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChtlElement(ScannerParser::ChtlElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementStart(ScannerParser::ElementStartContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementEnd(ScannerParser::ElementEndContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElementContent(ScannerParser::ElementContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChtlAttribute(ScannerParser::ChtlAttributeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChtlDeclaration(ScannerParser::ChtlDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChtlStatement(ScannerParser::ChtlStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTextBlock(ScannerParser::TextBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTextContent(ScannerParser::TextContentContext *ctx) override {
    return visitChildren(ctx);
  }


};

