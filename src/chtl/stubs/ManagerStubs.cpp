// Minimal stub implementations for managers to allow compilation
#include "../CHTLConstraint.h"
#include "../CHTLImport.h"
#include "../CHTLNamespace.h"
#include "../CHTLScript.h"
#include "../CHTLCMOD.h"
#include "../CHTLOrigin.h"
#include "../CHTLGenerator.h"
#include <iostream>

namespace chtl {

// ConstraintManager、ConstraintHelper、ConstraintProcessor 和 ConstraintRule 已移至 CHTLConstraint.cpp

// ========== ImportManager ==========
// 临时实现，直到 CHTLImport.cpp 能够编译（需要ANTLR生成的CHTL解析器）
ImportManager::ImportManager(std::shared_ptr<CHTLContext> ctx) : context(ctx) {}

bool ImportManager::processImport(const ImportDeclaration&, CHTLGenerator&) {
    return true;
}

bool ImportManager::processImports(const std::vector<ImportDeclaration>&, CHTLGenerator&) {
    return true;
}

void ImportManager::configurePaths(const ImportPathConfig&) {}

// ========== ImportProcessor ==========
ImportDeclaration ImportProcessor::parseImportStatement(const std::string&) {
    return ImportDeclaration();
}

// NamespaceManager、NamespaceProcessor 和 NamespaceResolver 已移至 CHTLNamespace.cpp

// ScriptManager 和 ScriptProcessor 已移至 CHTLScript.cpp

// CMODManager 已移至 CHTLCMOD.cpp

// OriginManager 已移至 CHTLOrigin.cpp

// OriginProcessor 已移至 CHTLOrigin.cpp

// ========== TemplateManager ==========
void TemplateManager::processInheritance() {}

// OriginDefinition 已移至 CHTLOrigin.cpp

} // namespace chtl