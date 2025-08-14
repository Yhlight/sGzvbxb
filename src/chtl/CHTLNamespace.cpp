#include "CHTLNamespace.h"
#include "CHTLGenerator.h"
#include "CHTLTemplate.h"
#include "CHTLCustom.h"
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {



// CHTLNamespaceDefinition 实现
std::shared_ptr<CHTLNamespaceDefinition> CHTLNamespaceDefinition::addChildNamespace(const std::string& childName) {
    auto it = childNamespaces.find(childName);
    if (it != childNamespaces.end()) {
        return it->second;
    }
    
    std::string childFullPath = fullPath.empty() ? childName : fullPath + "." + childName;
    auto child = std::make_shared<CHTLNamespaceDefinition>(childName, childFullPath);
    child->setParent(shared_from_this());
    childNamespaces[childName] = child;
    
    return child;
}

std::shared_ptr<CHTLNamespaceDefinition> CHTLNamespaceDefinition::getChildNamespace(const std::string& childName) const {
    auto it = childNamespaces.find(childName);
    return (it != childNamespaces.end()) ? it->second : nullptr;
}

std::vector<std::string> CHTLNamespaceDefinition::getChildNamespaceNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : childNamespaces) {
        names.push_back(name);
    }
    return names;
}

void CHTLNamespaceDefinition::addItem(const NamespaceItem& item) {
    items[item.name].push_back(item);
}

std::vector<NamespaceItem> CHTLNamespaceDefinition::getItems(const std::string& itemName) const {
    auto it = items.find(itemName);
    return (it != items.end()) ? it->second : std::vector<NamespaceItem>();
}

std::vector<NamespaceItem> CHTLNamespaceDefinition::getAllItems() const {
    std::vector<NamespaceItem> allItems;
    for (const auto& [_, itemList] : items) {
        allItems.insert(allItems.end(), itemList.begin(), itemList.end());
    }
    return allItems;
}

bool CHTLNamespaceDefinition::hasItem(const std::string& itemName) const {
    return items.find(itemName) != items.end();
}

std::shared_ptr<void> CHTLNamespaceDefinition::findItem(const std::string& itemName, NamespaceItemType type) const {
    auto itemList = getItems(itemName);
    for (const auto& item : itemList) {
        if (item.type == type) {
            return item.definition;
        }
    }
    return nullptr;
}

bool CHTLNamespaceDefinition::merge(const CHTLNamespaceDefinition& other, std::vector<std::string>& conflicts) {
    // 合并项目
    for (const auto& [itemName, otherItems] : other.items) {
        auto& myItems = items[itemName];
        
        for (const auto& otherItem : otherItems) {
            // 检查是否已存在相同类型的项目
            bool conflict = false;
            for (const auto& myItem : myItems) {
                if (myItem.type == otherItem.type) {
                    conflicts.push_back("Conflict in namespace '" + fullPath + 
                                      "': " + NamespaceHelper::itemTypeToString(myItem.type) + 
                                      " '" + itemName + "' already exists");
                    conflict = true;
                    break;
                }
            }
            
            if (!conflict) {
                myItems.push_back(otherItem);
            }
        }
    }
    
    // 合并子命名空间
    for (const auto& [childName, otherChild] : other.childNamespaces) {
        auto myChild = getChildNamespace(childName);
        if (!myChild) {
            // 创建新的子命名空间
            myChild = addChildNamespace(childName);
        }
        
        // 递归合并
        myChild->merge(*otherChild, conflicts);
    }
    
    return conflicts.empty();
}

// NamespaceManager 实现
NamespaceManager::NamespaceManager(std::shared_ptr<CHTLContext> ctx) : context(ctx) {
    globalNamespace = std::make_shared<CHTLNamespaceDefinition>("", "");
    namespaceIndex[""] = globalNamespace;
    namespaceStack.push_back(globalNamespace);
}

void NamespaceManager::beginNamespace(const std::string& name) {
    auto current = namespaceStack.empty() ? globalNamespace : namespaceStack.back();
    auto newNamespace = current->getChildNamespace(name);
    
    if (!newNamespace) {
        newNamespace = current->addChildNamespace(name);
        namespaceIndex[newNamespace->getFullPath()] = newNamespace;
    }
    
    namespaceStack.push_back(newNamespace);
}

void NamespaceManager::endNamespace() {
    if (namespaceStack.size() > 1) {
        namespaceStack.pop_back();
    } else {
        context->reportError("Cannot end namespace: already at global scope");
    }
}

std::shared_ptr<NamespaceDefinition> NamespaceManager::getCurrentNamespace() const {
    // TODO: Create adapter from CHTLNamespaceDefinition to NamespaceDefinition
    // For now, return nullptr since the types don't match
    return nullptr;
}

std::shared_ptr<NamespaceDefinition> NamespaceManager::getOrCreateNamespace(const std::string& path) {
    // TODO: Implement adapter from CHTLNamespaceDefinition to NamespaceDefinition
    return nullptr;
}

std::shared_ptr<NamespaceDefinition> NamespaceManager::findNamespace(const std::string& path) const {
    // TODO: Implement adapter from CHTLNamespaceDefinition to NamespaceDefinition
    return nullptr;
}

void NamespaceManager::registerItem(const std::string& itemName, NamespaceItemType type, 
                                   std::shared_ptr<void> definition) {
    auto current = namespaceStack.empty() ? globalNamespace : namespaceStack.back();
    
    NamespaceItem item;
    item.name = itemName;
    item.type = type;
    item.definition = definition;
    item.fullPath = current->getFullPath().empty() ? itemName : 
                   current->getFullPath() + "." + itemName;
    
    current->addItem(item);
}

std::shared_ptr<void> NamespaceManager::findItemInNamespace(const std::string& itemName, 
                                                           const std::string& namespacePath,
                                                           NamespaceItemType type) const {
    // 直接使用 CHTLNamespaceDefinition
    auto it = namespaceIndex.find(namespacePath);
    if (it != namespaceIndex.end()) {
        return it->second->findItem(itemName, type);
    }
    
    // 尝试文件默认命名空间
    for (const auto& [file, defaultNs] : fileDefaultNamespaces) {
        if (defaultNs == namespacePath) {
            auto nsIt = namespaceIndex.find(defaultNs);
            if (nsIt != namespaceIndex.end()) {
                return nsIt->second->findItem(itemName, type);
            }
            break;
        }
    }
    
    return nullptr;
}

void NamespaceManager::setFileDefaultNamespace(const std::string& filePath, const std::string& namespaceName) {
    fileDefaultNamespaces[filePath] = namespaceName;
}

std::string NamespaceManager::getFileDefaultNamespace(const std::string& filePath) const {
    auto it = fileDefaultNamespaces.find(filePath);
    if (it != fileDefaultNamespaces.end()) {
        return it->second;
    }
    
    // 如果没有显式命名空间，使用文件名作为默认命名空间
    return NamespaceHelper::generateDefaultNamespace(filePath);
}

bool NamespaceManager::mergeNamespaces(const std::string& targetPath, const std::string& sourcePath) {
    // 直接使用 CHTLNamespaceDefinition
    auto targetIt = namespaceIndex.find(targetPath);
    auto sourceIt = namespaceIndex.find(sourcePath);
    
    if (targetIt == namespaceIndex.end() || sourceIt == namespaceIndex.end()) {
        context->reportError("Cannot merge namespaces: namespace not found");
        return false;
    }
    
    std::vector<std::string> conflicts;
    bool success = targetIt->second->merge(*sourceIt->second, conflicts);
    
    // 报告冲突
    for (const auto& conflict : conflicts) {
        context->reportError(conflict);
    }
    
    return success;
}

std::vector<std::string> NamespaceManager::detectConflicts(const std::string& namespacePath) const {
    std::vector<std::string> conflicts;
    
    // 直接使用 CHTLNamespaceDefinition
    auto it = namespaceIndex.find(namespacePath);
    if (it == namespaceIndex.end()) {
        return conflicts;
    }
    
    auto ns = it->second;
    
    // 使用 getAllItems() 方法获取所有项目
    auto allItems = ns->getAllItems();
    std::unordered_map<std::string, std::vector<NamespaceItem>> itemsByName;
    
    for (const auto& item : allItems) {
        itemsByName[item.name].push_back(item);
    }
    
    // 检查同名不同类型的项目
    for (const auto& [itemName, itemList] : itemsByName) {
        if (itemList.size() > 1) {
            std::set<NamespaceItemType> types;
            for (const auto& item : itemList) {
                types.insert(item.type);
            }
            
            if (types.size() > 1) {
                conflicts.push_back("Multiple definitions of '" + itemName + 
                                  "' with different types in namespace '" + namespacePath + "'");
            }
        }
    }
    
    return conflicts;
}

void NamespaceManager::clear() {
    namespaceIndex.clear();
    namespaceStack.clear();
    fileDefaultNamespaces.clear();
    
    globalNamespace = std::make_shared<CHTLNamespaceDefinition>("", "");
    namespaceIndex[""] = globalNamespace;
    namespaceStack.push_back(globalNamespace);
}

// NamespaceResolver 实现
NamespaceResolver::NamespaceReference NamespaceResolver::parseReference(const std::string& reference) {
    NamespaceReference ref;
    ref.hasExplicitNamespace = false;
    
    // 解析 "ItemName from namespace.path" 格式
    std::regex fromPattern(R"((\w+)\s+from\s+([\w\.]+))");
    std::smatch match;
    
    if (std::regex_match(reference, match, fromPattern)) {
        ref.itemName = match[1];
        ref.namespacePath = match[2];
        ref.hasExplicitNamespace = true;
    } else {
        // 只有项目名，没有from子句
        ref.itemName = reference;
        ref.namespacePath = "";
    }
    
    return ref;
}

std::vector<std::string> NamespaceResolver::parseNamespacePath(const std::string& path) {
    return NamespaceHelper::splitPath(path);
}

std::string NamespaceResolver::buildFullPath(const std::vector<std::string>& components) {
    return NamespaceHelper::joinPath(components);
}

std::shared_ptr<void> NamespaceResolver::resolveItem(const std::string& itemName, 
                                                    NamespaceItemType type,
                                                    const std::string& fromNamespace) {
    if (!fromNamespace.empty()) {
        // 显式指定的命名空间
        return manager.findItemInNamespace(itemName, fromNamespace, type);
    }
    
    // 在当前命名空间及其父命名空间中查找
    auto current = manager.getCurrentCHTLNamespace();
    while (current) {
        auto item = current->findItem(itemName, type);
        if (item) {
            return item;
        }
        current = current->getParent();
    }
    
    return nullptr;
}

// NamespaceProcessor 实现
void NamespaceProcessor::processNamespaceDeclaration(const std::string& declaration) {
    auto block = parseNamespaceBlock(declaration);
    
    if (block.name.empty()) {
        return;
    }
    
    manager.beginNamespace(block.name);
    
    // 处理嵌套的命名空间声明
    for (const auto& nested : block.nestedDeclarations) {
        processNamespaceDeclaration(nested);
    }
}

bool NamespaceProcessor::processNamespacedUse(const std::string& useStatement, NamespaceItemType expectedType) {
    NamespaceResolver resolver(manager);
    auto ref = resolver.parseReference(useStatement);
    
    auto item = resolver.resolveItem(ref.itemName, expectedType, ref.namespacePath);
    if (!item) {
        return false;
    }
    
    // 根据类型使用项目
    std::string fullName = ref.hasExplicitNamespace ? 
        ref.namespacePath + "." + ref.itemName : ref.itemName;
    
    switch (expectedType) {
        case NamespaceItemType::CUSTOM_ELEMENT:
            // 使用自定义元素
            if (generator.getCustomManager()) {
                auto custom = generator.getCustomManager()->findCustomElement(fullName);
                if (custom) {
                    custom->expand(generator);
                }
            }
            break;
            
        case NamespaceItemType::TEMPLATE_STYLE:
            // 使用模板样式  
            if (generator.getTemplateManager()) {
                auto tmpl = generator.getTemplateManager()->findStyleTemplate(fullName);
                if (tmpl) {
                    generator.getTemplateManager()->useStyleTemplate(fullName, generator);
                }
            }
            break;
            
        case NamespaceItemType::TEMPLATE_ELEMENT:
            // 使用模板元素
            if (generator.getTemplateManager()) {
                auto tmpl = generator.getTemplateManager()->findElementTemplate(fullName);
                if (tmpl) {
                    generator.getTemplateManager()->useElementTemplate(fullName, generator);
                }
            }
            break;
            
        case NamespaceItemType::TEMPLATE_VAR:
            // 使用模板变量
            if (generator.getTemplateManager()) {
                // 变量使用通常需要具体的变量名
                // 这里只是注册到上下文，实际使用在变量引用时
            }
            break;
            
        case NamespaceItemType::CUSTOM_STYLE:
            // 使用自定义样式
            if (generator.getCustomManager()) {
                generator.getCustomManager()->useCustomStyle(fullName, generator);
            }
            break;
            
        case NamespaceItemType::CUSTOM_VAR:
            // 使用自定义变量
            // 变量使用通常需要具体的变量名和值
            // 这里只是注册到上下文，实际使用在变量引用时
            break;
            
        default:
            break;
    }
    
    return true;
}

NamespaceProcessor::NamespaceBlock NamespaceProcessor::parseNamespaceBlock(const std::string& declaration) {
    NamespaceBlock block;
    
    // 解析命名空间声明
    // [Namespace] name
    // [Namespace] name { ... }
    std::regex simplePattern(R"(\[Namespace\]\s+(\w+))");
    std::regex blockPattern(R"(\[Namespace\]\s+(\w+)\s*\{)");
    
    std::smatch match;
    
    if (std::regex_match(declaration, match, blockPattern)) {
        block.name = match[1];
        block.hasBody = true;
    } else if (std::regex_match(declaration, match, simplePattern)) {
        block.name = match[1];
        block.hasBody = false;
    }
    
    return block;
}

// NamespaceMerger 实现
bool NamespaceMerger::merge(NamespaceDefinition& target, 
                           const NamespaceDefinition& source,
                           const MergeOptions& options,
                           std::vector<std::string>& conflicts) {
    // 实现已在NamespaceDefinition::merge中
    return target.merge(source, conflicts);
}

std::vector<std::string> NamespaceMerger::detectPotentialConflicts(
    const NamespaceDefinition& ns1,
    const NamespaceDefinition& ns2) {
    std::vector<std::string> conflicts;
    
    // 检查项目冲突
    for (const auto& [itemName, items1] : ns1.items) {
        auto items2 = ns2.getItems(itemName);
        
        for (const auto& item1 : items1) {
            for (const auto& item2 : items2) {
                if (item1.type == item2.type) {
                    conflicts.push_back("Potential conflict: " + 
                                      NamespaceHelper::itemTypeToString(item1.type) + 
                                      " '" + itemName + "' exists in both namespaces");
                }
            }
        }
    }
    
    return conflicts;
}

NamespaceMerger::ConflictResolution NamespaceMerger::resolveConflict(const NamespaceItem& item1,
                                                                   const NamespaceItem& item2) {
    // 简单策略：如果来自同一文件，保留第二个；否则报错
    if (item1.sourceFile == item2.sourceFile) {
        return ConflictResolution::KEEP_SECOND;
    }
    
    return ConflictResolution::ERROR;
}

// NamespaceHelper 实现
namespace NamespaceHelper {

bool isValidNamespaceName(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    
    // 必须以字母或下划线开头，后续可以是字母、数字或下划线
    std::regex pattern("^[a-zA-Z_][a-zA-Z0-9_]*$");
    return std::regex_match(name, pattern);
}

std::vector<std::string> splitPath(const std::string& path) {
    std::vector<std::string> components;
    std::stringstream ss(path);
    std::string component;
    
    while (std::getline(ss, component, '.')) {
        if (!component.empty()) {
            components.push_back(component);
        }
    }
    
    return components;
}

std::string joinPath(const std::vector<std::string>& components) {
    std::string path;
    for (size_t i = 0; i < components.size(); ++i) {
        if (i > 0) {
            path += ".";
        }
        path += components[i];
    }
    return path;
}

std::string itemTypeToString(NamespaceItemType type) {
    switch (type) {
        case NamespaceItemType::CUSTOM_ELEMENT: return "Custom Element";
        case NamespaceItemType::CUSTOM_STYLE: return "Custom Style";
        case NamespaceItemType::CUSTOM_VAR: return "Custom Variable Group";
        case NamespaceItemType::TEMPLATE_ELEMENT: return "Template Element";
        case NamespaceItemType::TEMPLATE_STYLE: return "Template Style";
        case NamespaceItemType::TEMPLATE_VAR: return "Template Variable Group";
        case NamespaceItemType::NESTED_NAMESPACE: return "Nested Namespace";
        default: return "Unknown";
    }
}

std::string generateDefaultNamespace(const std::string& filePath) {
    // 从文件路径提取文件名（不含扩展名）作为默认命名空间
    size_t lastSlash = filePath.find_last_of("/\\");
    size_t lastDot = filePath.find_last_of('.');
    
    std::string filename;
    if (lastSlash != std::string::npos) {
        filename = filePath.substr(lastSlash + 1);
    } else {
        filename = filePath;
    }
    
    if (lastDot != std::string::npos && lastDot > lastSlash) {
        filename = filename.substr(0, lastDot - (lastSlash + 1));
    }
    
    // 确保是有效的命名空间名
    std::replace(filename.begin(), filename.end(), '-', '_');
    std::replace(filename.begin(), filename.end(), ' ', '_');
    
    return filename;
}

bool isReservedNamespace(const std::string& name) {
    static const std::set<std::string> reserved = {
        "global", "system", "std", "chtl", "import", "export",
        "namespace", "template", "custom", "origin"
    };
    
    return reserved.find(name) != reserved.end();
}

} // namespace NamespaceHelper

} // namespace chtl