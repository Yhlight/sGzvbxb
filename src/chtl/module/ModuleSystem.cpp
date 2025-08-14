#include "ModuleSystem.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <queue>
#include "../error/ErrorInterface.h"

namespace chtl {
namespace module {

namespace fs = std::filesystem;

// ModuleInfo 实现
ModuleInfo::ModuleInfo(const std::string& path, ModuleType type)
    : path_(path), type_(type) {
}

void ModuleInfo::addImport(const ImportItem& item) {
    imports_.push_back(item);
    if (!item.sourcePath.empty()) {
        dependencies_.insert(item.sourcePath);
    }
}

void ModuleInfo::addExport(const ExportItem& item) {
    exports_.push_back(item);
}

// ModuleParser 实现
ModuleParser::ModuleParser() {}

std::shared_ptr<ModuleInfo> ModuleParser::parseModule(const std::string& path) {
    auto moduleType = detectModuleType(path);
    auto module = std::make_shared<ModuleInfo>(path, moduleType);
    
    // 读取文件内容
    std::ifstream file(path);
    if (!file.is_open()) {
        error::ErrorInfo err(error::ErrorLevel::Error, 
                            error::ErrorCategory::FileIO,
                            "无法打开模块文件: " + path);
        throw error::CHTLException(err);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    module->setCachedContent(content);
    
    // 提取命名空间
    std::string ns = extractNamespace(content);
    if (!ns.empty()) {
        module->setNamespace(ns);
    } else if (moduleType == ModuleType::CHTL) {
        // 如果没有显式命名空间，使用文件名
        fs::path filePath(path);
        module->setNamespace(filePath.stem().string());
    }
    
    // 解析导入语句
    std::regex importRegex(R"(\[Import\]([^;]+);?)");
    std::smatch match;
    std::string::const_iterator searchStart(content.cbegin());
    
    while (std::regex_search(searchStart, content.cend(), match, importRegex)) {
        std::string importStmt = match[0];
        ImportItem item = parseImportStatement(importStmt);
        module->addImport(item);
        searchStart = match.suffix().first;
    }
    
    // 解析导出块
    std::regex exportRegex(R"(\[Export\]\s*\{([^}]+)\})");
    if (std::regex_search(content, match, exportRegex)) {
        std::string exportBlock = match[1];
        auto exports = parseExportBlock(exportBlock);
        for (const auto& exp : exports) {
            module->addExport(exp);
        }
    }
    
    return module;
}

ImportItem ModuleParser::parseImportStatement(const std::string& statement) {
    ImportItem item;
    item.isTemplate = false;
    item.isCustom = false;
    
    // 检查是否是[Template]或[Custom]
    if (statement.find("[Template]") != std::string::npos) {
        item.isTemplate = true;
    }
    if (statement.find("[Custom]") != std::string::npos) {
        item.isCustom = true;
    }
    
    // 解析导入类型
    if (statement.find("@Element") != std::string::npos) {
        item.type = ImportItemType::Element;
        // 提取元素名称
        std::regex nameRegex(R"(@Element\s+(\w+))");
        std::smatch match;
        if (std::regex_search(statement, match, nameRegex)) {
            item.itemName = match[1];
        }
    } else if (statement.find("@Style") != std::string::npos && 
               statement.find("from") != std::string::npos) {
        item.type = ImportItemType::Style;
        // 提取样式组名称
        std::regex nameRegex(R"(@Style\s+(\w+))");
        std::smatch match;
        if (std::regex_search(statement, match, nameRegex)) {
            item.itemName = match[1];
        }
    } else if (statement.find("@Var") != std::string::npos) {
        item.type = ImportItemType::Var;
        // 提取变量组名称
        std::regex nameRegex(R"(@Var\s+(\w+))");
        std::smatch match;
        if (std::regex_search(statement, match, nameRegex)) {
            item.itemName = match[1];
        }
    } else if (statement.find("@Html") != std::string::npos) {
        item.type = ImportItemType::Html;
    } else if (statement.find("@JavaScript") != std::string::npos) {
        item.type = ImportItemType::JavaScript;
    } else if (statement.find("@Chtl") != std::string::npos) {
        item.type = ImportItemType::Chtl;
    } else if (statement.find("@CJmod") != std::string::npos) {
        item.type = ImportItemType::CJmod;
    }
    
    // 提取源路径
    std::regex fromRegex(R"(from\s+([^\s]+))");
    std::smatch fromMatch;
    if (std::regex_search(statement, fromMatch, fromRegex)) {
        std::string path = fromMatch[1];
        // 将.替换为/
        std::replace(path.begin(), path.end(), '.', '/');
        item.sourcePath = path;
    }
    
    // 提取别名
    std::regex asRegex(R"(as\s+(\w+))");
    std::smatch asMatch;
    if (std::regex_search(statement, asMatch, asRegex)) {
        item.alias = asMatch[1];
    }
    
    return item;
}

std::vector<ExportItem> ModuleParser::parseExportBlock(const std::string& block) {
    std::vector<ExportItem> exports;
    
    // 解析每一行的导出项
    std::istringstream stream(block);
    std::string line;
    
    while (std::getline(stream, line)) {
        // 去除前后空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t,;") + 1);
        
        if (line.empty()) continue;
        
        ExportItem item;
        item.isTemplate = false;
        item.isCustom = false;
        
        // 检查类型
        if (line.find("@Style") != std::string::npos) {
            item.type = ImportItemType::Style;
            // 提取所有样式名
            std::regex nameRegex(R"(\w+)");
            std::sregex_iterator it(line.begin(), line.end(), nameRegex);
            std::sregex_iterator end;
            
            // 跳过@Style
            if (it != end) ++it;
            
            while (it != end) {
                ExportItem styleItem = item;
                styleItem.itemName = it->str();
                exports.push_back(styleItem);
                ++it;
            }
        } else if (line.find("@Element") != std::string::npos) {
            item.type = ImportItemType::Element;
            // 类似处理
        }
        // ... 其他类型
    }
    
    return exports;
}

std::string ModuleParser::extractNamespace(const std::string& content) {
    std::regex nsRegex(R"(\[Namespace\]\s+(\w+))");
    std::smatch match;
    if (std::regex_search(content, match, nsRegex)) {
        return match[1];
    }
    return "";
}

ModuleType ModuleParser::detectModuleType(const std::string& path) {
    fs::path filePath(path);
    std::string ext = filePath.extension().string();
    
    if (ext == ".chtl") {
        // 检查是否是CMOD或CJMOD
        fs::path parent = filePath.parent_path();
        if (parent.filename() == "src" && 
            fs::exists(parent.parent_path() / "CMOD.chtl")) {
            return ModuleType::CMOD;
        }
        if (parent.filename() == "src" && 
            fs::exists(parent.parent_path() / "CJMOD.chtl")) {
            return ModuleType::CJMOD;
        }
        return ModuleType::CHTL;
    } else if (ext == ".html" || ext == ".htm") {
        return ModuleType::HTML;
    } else if (ext == ".css") {
        return ModuleType::CSS;
    } else if (ext == ".js") {
        return ModuleType::JavaScript;
    }
    
    return ModuleType::CHTL;
}

// ModuleLoader 实现
ModuleLoader::ModuleLoader() {}

void ModuleLoader::addSearchPath(const std::string& path) {
    searchPaths_.push_back(path);
}

std::shared_ptr<ModuleInfo> ModuleLoader::loadModule(const std::string& path) {
    // 检查缓存
    auto it = moduleCache_.find(path);
    if (it != moduleCache_.end()) {
        return it->second;
    }
    
    // 查找文件
    std::string fullPath = findModuleFile(path);
    if (fullPath.empty()) {
        error::ErrorInfo err(error::ErrorLevel::Error, 
                            error::ErrorCategory::FileIO,
                            "找不到模块: " + path);
        throw error::CHTLException(err);
    }
    
    // 解析模块
    auto module = parser_.parseModule(fullPath);
    moduleCache_[path] = module;
    moduleCache_[fullPath] = module;  // 也用完整路径缓存
    
    return module;
}

std::string ModuleLoader::resolvePath(const std::string& importPath, 
                                     const std::string& currentFile) {
    std::string path = importPath;
    
    // 将.替换为/
    std::replace(path.begin(), path.end(), '.', '/');
    
    // 如果是相对路径
    if (path.find("./") == 0 || path.find("../") == 0) {
        fs::path current(currentFile);
        fs::path resolved = current.parent_path() / path;
        return resolved.lexically_normal().string();
    }
    
    return path;
}

std::string ModuleLoader::findModuleFile(const std::string& path) {
    // 首先检查是否是绝对路径
    if (fs::exists(path)) {
        return fs::absolute(path).string();
    }
    
    // 尝试添加.chtl扩展名
    std::vector<std::string> tryPaths = {
        path,
        path + ".chtl",
        path + "/index.chtl"  // 目录模块
    };
    
    // 在搜索路径中查找
    for (const auto& searchPath : searchPaths_) {
        for (const auto& tryPath : tryPaths) {
            fs::path fullPath = fs::path(searchPath) / tryPath;
            if (fs::exists(fullPath)) {
                return fullPath.lexically_normal().string();
            }
        }
    }
    
    return "";
}

std::string ModuleLoader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        error::ErrorInfo err(error::ErrorLevel::Error, 
                            error::ErrorCategory::FileIO,
                            "无法读取文件: " + path);
        throw error::CHTLException(err);
    }
    
    return std::string((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
}

// ModuleDependencyGraph 实现
ModuleDependencyGraph::ModuleDependencyGraph() {}

void ModuleDependencyGraph::addModule(std::shared_ptr<ModuleInfo> module) {
    const std::string& path = module->getPath();
    modules_[path] = module;
    
    // 构建邻接表
    for (const auto& dep : module->getDependencies()) {
        adjacencyList_[path].insert(dep);
    }
}

bool ModuleDependencyGraph::hasCircularDependency() const {
    std::set<std::string> visited;
    std::set<std::string> recursionStack;
    std::vector<std::string> path;
    
    for (const auto& [modulePath, _] : modules_) {
        if (visited.find(modulePath) == visited.end()) {
            if (dfsHasCycle(modulePath, visited, recursionStack, path)) {
                return true;
            }
        }
    }
    
    return false;
}

std::vector<std::string> ModuleDependencyGraph::findCircularPath() const {
    std::set<std::string> visited;
    std::set<std::string> recursionStack;
    std::vector<std::string> path;
    
    for (const auto& [modulePath, _] : modules_) {
        if (visited.find(modulePath) == visited.end()) {
            if (dfsHasCycle(modulePath, visited, recursionStack, path)) {
                return path;
            }
        }
    }
    
    return {};
}

std::vector<std::string> ModuleDependencyGraph::topologicalSort() const {
    // 计算入度
    std::unordered_map<std::string, int> inDegree;
    for (const auto& [node, _] : modules_) {
        inDegree[node] = 0;
    }
    
    for (const auto& [node, neighbors] : adjacencyList_) {
        for (const auto& neighbor : neighbors) {
            inDegree[neighbor]++;
        }
    }
    
    // 使用队列进行拓扑排序
    std::queue<std::string> queue;
    for (const auto& [node, degree] : inDegree) {
        if (degree == 0) {
            queue.push(node);
        }
    }
    
    std::vector<std::string> result;
    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop();
        result.push_back(current);
        
        auto it = adjacencyList_.find(current);
        if (it != adjacencyList_.end()) {
            for (const auto& neighbor : it->second) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    queue.push(neighbor);
                }
            }
        }
    }
    
    // 如果结果数量不等于节点数量，说明有环
    if (result.size() != modules_.size()) {
        error::ErrorInfo err(error::ErrorLevel::Error, 
                            error::ErrorCategory::ModuleSystem,
                            "模块依赖图中存在循环依赖");
        throw error::CHTLException(err);
    }
    
    return result;
}

bool ModuleDependencyGraph::dfsHasCycle(const std::string& node,
                                       std::set<std::string>& visited,
                                       std::set<std::string>& recursionStack,
                                       std::vector<std::string>& path) const {
    visited.insert(node);
    recursionStack.insert(node);
    path.push_back(node);
    
    auto it = adjacencyList_.find(node);
    if (it != adjacencyList_.end()) {
        for (const auto& neighbor : it->second) {
            if (recursionStack.find(neighbor) != recursionStack.end()) {
                // 找到环，截取环路径
                auto cycleStart = std::find(path.begin(), path.end(), neighbor);
                path.erase(path.begin(), cycleStart);
                path.push_back(neighbor);  // 添加回到起点
                return true;
            }
            
            if (visited.find(neighbor) == visited.end()) {
                if (dfsHasCycle(neighbor, visited, recursionStack, path)) {
                    return true;
                }
            }
        }
    }
    
    recursionStack.erase(node);
    path.pop_back();
    return false;
}

} // namespace module
} // namespace chtl