#include "../CHTLContext.h"
#include <algorithm>

namespace chtl {

bool Scope::addSymbol(std::shared_ptr<Symbol> symbol) {
    // 检查符号是否已存在
    if (symbols.find(symbol->name) != symbols.end()) {
        return false; // 符号已存在
    }
    
    symbols[symbol->name] = symbol;
    return true;
}

std::shared_ptr<Symbol> Scope::lookupSymbol(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Symbol> Scope::lookupSymbolRecursive(const std::string& name) {
    // 先在当前作用域查找
    auto symbol = lookupSymbol(name);
    if (symbol) {
        return symbol;
    }
    
    // 如果没找到，递归查找父作用域
    if (parent) {
        return parent->lookupSymbolRecursive(name);
    }
    
    return nullptr;
}

void Scope::addConstraint(const std::string& constraint) {
    constraints.push_back(constraint);
}

bool Scope::isAllowed(const std::string& feature) const {
    // 检查功能是否在约束列表中（如果在列表中，表示不允许）
    return std::find(constraints.begin(), constraints.end(), feature) == constraints.end();
}

} // namespace chtl