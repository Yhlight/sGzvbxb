#include "ImportEnhanced.h"
#include "CHTLGenerator.h"
#include "CHTLTemplate.h"
#include "CHTLCustom.h"
#include "CHTLOrigin.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {

// PathNormalizer 实现
std::filesystem::path PathNormalizer::normalize(const std::string& path, 
                                               const std::filesystem::path& basePath) {
    // 转换点号为斜杠
    std::string normalizedPath = convertDotsToSlashes(path);
    
    // 创建路径对象
    std::filesystem::path p(normalizedPath);
    
    // 如果是相对路径，基于basePath解析
    if (p.is_relative() && !basePath.empty()) {
        p = basePath / p;
    }
    
    // 规范化路径（解析 .. 和 . 等）
    return std::filesystem::weakly_canonical(p);
}

bool PathNormalizer::isFileName(const std::string& path) {
    // 不包含路径分隔符
    return path.find('/') == std::string::npos && 
           path.find('\\') == std::string::npos &&
           path.find('.') == std::string::npos;  // 也不包含点（用作路径分隔符）
}

bool PathNormalizer::hasExtension(const std::string& path) {
    // 获取最后一个点的位置
    size_t lastDot = path.find_last_of('.');
    if (lastDot == std::string::npos) {
        return false;
    }
    
    // 检查点后面是否还有路径分隔符
    size_t lastSlash = path.find_last_of("/\\");
    return lastSlash == std::string::npos || lastDot > lastSlash;
}

std::string PathNormalizer::convertDotsToSlashes(const std::string& path) {
    std::string result = path;
    
    // 保护文件扩展名中的点
    size_t lastDot = result.find_last_of('.');
    size_t lastSlash = result.find_last_of("/\\");
    
    // 如果最后的点在路径分隔符之后，说明是文件扩展名
    bool hasFileExtension = lastDot != std::string::npos && 
                           (lastSlash == std::string::npos || lastDot > lastSlash);
    
    if (hasFileExtension) {
        // 只替换扩展名之前的点
        for (size_t i = 0; i < lastDot; ++i) {
            if (result[i] == '.') {
                result[i] = '/';
            }
        }
    } else {
        // 替换所有的点
        std::replace(result.begin(), result.end(), '.', '/');
    }
    
    return result;
}

std::vector<std::string> PathNormalizer::parseWildcardPattern(const std::string& pattern) {
    std::vector<std::string> parts;
    
    // 分割路径和通配符部分
    size_t lastSlash = pattern.find_last_of("/\\.");
    if (lastSlash != std::string::npos) {
        parts.push_back(pattern.substr(0, lastSlash));
        parts.push_back(pattern.substr(lastSlash + 1));
    } else {
        parts.push_back("");
        parts.push_back(pattern);
    }
    
    return parts;
}

// FileSearcherEnhanced 实现
std::filesystem::path FileSearcherEnhanced::searchFile(const std::string& name, ImportType type) {
    switch (type) {
        case ImportType::HTML:
            return searchResourceFile(name, ".html", config.currentDirectory);
        case ImportType::STYLE:
            return searchResourceFile(name, ".css", config.currentDirectory);
        case ImportType::JAVASCRIPT:
            return searchResourceFile(name, ".js", config.currentDirectory);
        case ImportType::CHTL:
            return searchModuleFile(name, true);  // CMOD优先
        case ImportType::CJMOD:
            return searchCJMODFile(name);
        default:
            return {};
    }
}

std::filesystem::path FileSearcherEnhanced::searchResourceFile(const std::string& name, 
                                                              const std::string& extension,
                                                              const std::filesystem::path& currentDir) {
    // 如果已经有扩展名，直接使用
    if (PathNormalizer::hasExtension(name)) {
        auto path = currentDir / name;
        if (std::filesystem::exists(path)) {
            return path;
        }
        return {};
    }
    
    // 尝试添加扩展名
    auto pathWithExt = currentDir / (name + extension);
    if (std::filesystem::exists(pathWithExt)) {
        return pathWithExt;
    }
    
    return {};
}

std::filesystem::path FileSearcherEnhanced::searchModuleFile(const std::string& name, bool preferCMOD) {
    std::vector<std::filesystem::path> searchPaths;
    
    // 构建搜索路径列表
    if (config.searchOfficialFirst) {
        searchPaths.push_back(config.officialModulePath);
        searchPaths.push_back(config.moduleDirectory);
        searchPaths.push_back(config.currentDirectory);
    } else {
        searchPaths.push_back(config.moduleDirectory);
        searchPaths.push_back(config.currentDirectory);
        searchPaths.push_back(config.officialModulePath);
    }
    
    // 文件扩展名优先级
    std::vector<std::string> extensions;
    if (preferCMOD) {
        extensions = {".cmod", ".chtl"};
    } else {
        extensions = {".chtl", ".cmod"};
    }
    
    // 如果名称已有扩展名
    if (PathNormalizer::hasExtension(name)) {
        for (const auto& searchPath : searchPaths) {
            auto fullPath = searchPath / name;
            if (std::filesystem::exists(fullPath)) {
                return fullPath;
            }
        }
        return {};
    }
    
    // 尝试不同的扩展名
    for (const auto& searchPath : searchPaths) {
        for (const auto& ext : extensions) {
            auto fullPath = searchPath / (name + ext);
            if (std::filesystem::exists(fullPath)) {
                return fullPath;
            }
        }
    }
    
    return {};
}

std::filesystem::path FileSearcherEnhanced::searchCJMODFile(const std::string& name) {
    std::vector<std::filesystem::path> searchPaths = {
        config.officialModulePath,
        config.moduleDirectory,
        config.currentDirectory
    };
    
    // 如果名称已有扩展名
    if (PathNormalizer::hasExtension(name)) {
        for (const auto& searchPath : searchPaths) {
            auto fullPath = searchPath / name;
            if (std::filesystem::exists(fullPath)) {
                return fullPath;
            }
        }
        return {};
    }
    
    // 尝试添加 .cjmod 扩展名
    for (const auto& searchPath : searchPaths) {
        auto fullPath = searchPath / (name + ".cjmod");
        if (std::filesystem::exists(fullPath)) {
            return fullPath;
        }
    }
    
    return {};
}

std::vector<std::filesystem::path> FileSearcherEnhanced::searchWildcard(const std::string& pattern, 
                                                                      const std::filesystem::path& basePath) {
    std::vector<std::filesystem::path> results;
    
    // 解析通配符模式
    auto parts = PathNormalizer::parseWildcardPattern(pattern);
    std::filesystem::path searchDir = basePath;
    if (!parts[0].empty()) {
        searchDir = basePath / parts[0];
    }
    
    if (!std::filesystem::exists(searchDir) || !std::filesystem::is_directory(searchDir)) {
        return results;
    }
    
    // 构建正则表达式
    std::string wildcardPattern = parts[1];
    std::string regexPattern = wildcardPattern;
    
    // 转换通配符为正则表达式
    regexPattern = std::regex_replace(regexPattern, std::regex("\\."), "\\.");
    regexPattern = std::regex_replace(regexPattern, std::regex("\\*"), ".*");
    regexPattern = "^" + regexPattern + "$";
    
    std::regex fileRegex(regexPattern);
    
    // 搜索匹配的文件
    for (const auto& entry : std::filesystem::directory_iterator(searchDir)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, fileRegex)) {
                results.push_back(entry.path());
            }
        }
    }
    
    return results;
}

// CircularDependencyDetectorImpl 实现
bool CircularDependencyDetectorImpl::beginProcessing(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // 检查是否已在处理栈中（循环依赖）
    if (std::find(processingStack.begin(), processingStack.end(), filePath) != processingStack.end()) {
        return false;  // 检测到循环依赖
    }
    
    processingStack.push_back(filePath);
    return true;
}

void CircularDependencyDetectorImpl::endProcessing(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // 从处理栈中移除
    auto it = std::find(processingStack.begin(), processingStack.end(), filePath);
    if (it != processingStack.end()) {
        processingStack.erase(it);
    }
    
    processed.insert(filePath);
}

void CircularDependencyDetectorImpl::addDependency(const std::string& from, const std::string& to) {
    std::lock_guard<std::mutex> lock(mutex);
    dependencyGraph[from].insert(to);
}

bool CircularDependencyDetectorImpl::hasCircularDependency(const std::string& from, 
                                                          const std::string& to) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::unordered_set<std::string> visited;
    std::vector<std::string> path;
    
    return dfsDetectCycle(from, to, visited, path);
}

std::vector<std::string> CircularDependencyDetectorImpl::getCircularPath(const std::string& from, 
                                                                        const std::string& to) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::unordered_set<std::string> visited;
    std::vector<std::string> path;
    
    if (dfsDetectCycle(from, to, visited, path)) {
        return path;
    }
    
    return {};
}

void CircularDependencyDetectorImpl::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    dependencyGraph.clear();
    processingStack.clear();
    processed.clear();
}

bool CircularDependencyDetectorImpl::dfsDetectCycle(const std::string& current, 
                                                   const std::string& target,
                                                   std::unordered_set<std::string>& visited,
                                                   std::vector<std::string>& path) const {
    if (current == target && !path.empty()) {
        return true;  // 找到循环
    }
    
    if (visited.find(current) != visited.end()) {
        return false;  // 已访问
    }
    
    visited.insert(current);
    path.push_back(current);
    
    auto it = dependencyGraph.find(current);
    if (it != dependencyGraph.end()) {
        for (const auto& dep : it->second) {
            if (dfsDetectCycle(dep, target, visited, path)) {
                return true;
            }
        }
    }
    
    path.pop_back();
    return false;
}

// ImportManagerEnhanced 实现
ImportManagerEnhanced::ImportManagerEnhanced(std::shared_ptr<CHTLContext> ctx) 
    : ImportManager(ctx),
      fileSearcher(std::make_unique<FileSearcherEnhanced>(pathConfig)) {
    dependencyDetector = std::make_unique<CircularDependencyDetectorImpl>();
}

bool ImportManagerEnhanced::processImport(const ImportDeclaration& decl, 
                                         CHTLGenerator& generator) {
    // 转换为 shared_ptr 以便在内部使用
    auto generatorPtr = std::shared_ptr<CHTLGenerator>(&generator, [](CHTLGenerator*){});
    // 规范化路径
    auto normalizedPath = normalizePath(decl.sourcePath);
    
    // 检查循环依赖
    std::string currentFile = context->getCurrentFile();
    if (!dependencyDetector->beginProcessing(currentFile)) {
        auto circularPath = dependencyDetector->getCircularPath(currentFile, normalizedPath.string());
        std::string errorMsg = "Circular dependency detected: ";
        for (size_t i = 0; i < circularPath.size(); ++i) {
            if (i > 0) errorMsg += " -> ";
            errorMsg += circularPath[i];
        }
        context->reportError(errorMsg);
        return false;
    }
    
    // 添加依赖关系
    dependencyDetector->addDependency(currentFile, normalizedPath.string());
    
    // 检查是否已导入
    if (isAlreadyImported(normalizedPath)) {
        dependencyDetector->endProcessing(currentFile);
        return true;  // 已导入，跳过
    }
    
    bool result = false;
    
    // 根据类型处理导入
    switch (decl.type) {
        case ImportType::HTML:
        case ImportType::STYLE:
        case ImportType::JAVASCRIPT:
            result = processResourceImport(decl, generatorPtr);
            break;
            
        case ImportType::CHTL:
            if (decl.isWildcard) {
                result = processWildcardImport(decl, generatorPtr);
            } else {
                result = processModuleImport(decl, generatorPtr);
            }
            break;
            
        case ImportType::CJMOD:
            result = processCJMODImport(decl, generatorPtr);
            break;
            
        default:
            // 其他类型由基类处理
            result = ImportManager::processImport(decl, generator);
            break;
    }
    
    if (result) {
        markAsImported(normalizedPath);
    }
    
    dependencyDetector->endProcessing(currentFile);
    return result;
}

bool ImportManagerEnhanced::processResourceImport(const ImportDeclaration& decl,
                                                 std::shared_ptr<CHTLGenerator> generator) {
    // 搜索文件
    auto filePath = fileSearcher->searchFile(decl.sourcePath, decl.type);
    if (filePath.empty()) {
        context->reportError("Cannot find resource file: " + decl.sourcePath);
        return false;
    }
    
    // 如果没有 as 子句，跳过
    if (!decl.hasAsClause) {
        return true;
    }
    
    // 加载文件内容
    std::ifstream file(filePath);
    if (!file) {
        context->reportError("Cannot open file: " + filePath.string());
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // 创建原始嵌入节点
    createOriginNode(content, decl.asName, decl.type, generator);
    
    return true;
}

bool ImportManagerEnhanced::processModuleImport(const ImportDeclaration& decl,
                                               std::shared_ptr<CHTLGenerator> generator) {
    // 处理子模块导入 (e.g., Chtholly.Space)
    size_t dotPos = decl.sourcePath.find('.');
    if (dotPos != std::string::npos && !PathNormalizer::hasExtension(decl.sourcePath)) {
        std::string moduleName = decl.sourcePath.substr(0, dotPos);
        std::string submodulePath = decl.sourcePath.substr(dotPos + 1);
        
        ImportProcessorEnhanced processor(std::static_pointer_cast<ImportManagerEnhanced>(shared_from_this()), context);
        return processor.processSubmoduleImport(moduleName, submodulePath);
    }
    
    // 搜索模块文件
    auto filePath = fileSearcher->searchFile(decl.sourcePath, ImportType::CHTL);
    if (filePath.empty()) {
        context->reportError("Cannot find module: " + decl.sourcePath);
        return false;
    }
    
    // 处理模块文件
    // TODO: 实际的模块处理逻辑
    return true;
}

bool ImportManagerEnhanced::processCJMODImport(const ImportDeclaration& decl,
                                              std::shared_ptr<CHTLGenerator> generator) {
    // 搜索 CJMOD 文件
    auto filePath = fileSearcher->searchFile(decl.sourcePath, ImportType::CJMOD);
    if (filePath.empty()) {
        context->reportError("Cannot find CJMOD: " + decl.sourcePath);
        return false;
    }
    
    // 处理 CJMOD 文件
    // TODO: 实际的 CJMOD 处理逻辑
    return true;
}

bool ImportManagerEnhanced::processWildcardImport(const ImportDeclaration& decl,
                                                  std::shared_ptr<CHTLGenerator> generator) {
    // 解析基础路径和通配符模式
    auto normalizedPath = PathNormalizer::convertDotsToSlashes(decl.sourcePath);
    auto files = fileSearcher->searchWildcard(normalizedPath, pathConfig.currentDirectory);
    
    if (files.empty()) {
        context->reportWarning("No files match pattern: " + decl.sourcePath);
        return true;  // 不是错误
    }
    
    // 导入所有匹配的文件
    bool allSuccess = true;
    for (const auto& file : files) {
        ImportDeclaration fileImport = decl;
        fileImport.sourcePath = file.string();
        fileImport.isWildcard = false;
        
        if (!processModuleImport(fileImport, generator)) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

void ImportManagerEnhanced::createOriginNode(const std::string& content, 
                                            const std::string& asName,
                                            ImportType type,
                                            std::shared_ptr<CHTLGenerator> generator) {
    // 根据类型创建相应的原始嵌入节点
    std::string syntaxType;
    switch (type) {
        case ImportType::HTML:
            syntaxType = "@Html";
            break;
        case ImportType::STYLE:
            syntaxType = "@Style";
            break;
        case ImportType::JAVASCRIPT:
            syntaxType = "@JavaScript";
            break;
        default:
            return;  // 不支持的类型
    }
    
    // 创建原始嵌入定义
    auto originDef = std::make_shared<OriginDefinition>(asName, syntaxType, true);
    originDef->setContent(content);
    
    // 注册到原始嵌入管理器
    if (originManager) {
        originManager->registerNamedOrigin(asName, originDef);
    }
    
    // TODO: 将原始嵌入节点添加到生成器
}

std::filesystem::path ImportManagerEnhanced::normalizePath(const std::string& path) const {
    std::lock_guard<std::mutex> lock(pathCacheMutex);
    
    // 检查缓存
    auto it = pathCache.find(path);
    if (it != pathCache.end()) {
        return it->second;
    }
    
    // 规范化路径
    auto normalized = PathNormalizer::normalize(path, pathConfig.currentDirectory);
    pathCache[path] = normalized;
    
    return normalized;
}

bool ImportManagerEnhanced::isAlreadyImported(const std::filesystem::path& normalizedPath) const {
    return importedFiles.find(normalizedPath.string()) != importedFiles.end();
}

void ImportManagerEnhanced::markAsImported(const std::filesystem::path& normalizedPath) {
    importedFiles.insert(normalizedPath.string());
}

// ImportProcessorEnhanced 实现
ImportDeclaration ImportProcessorEnhanced::parseImportStatement(const std::string& statement) {
    ImportDeclaration decl;
    
    // 使用正则表达式解析导入语句
    // [Import] @Type from "path" as name
    std::regex importRegex("\\[Import\\]\\s+(@\\w+|\\[\\w+\\]\\s+@\\w+(?:\\s+\\w+)?)\\s+from\\s+\"([^\"]+)\"(?:\\s+as\\s+(\\w+))?");
    std::smatch match;
    
    if (std::regex_match(statement, match, importRegex)) {
        std::string typeStr = match[1];
        decl.sourcePath = match[2];
        if (match[3].matched) {
            decl.asName = match[3];
            decl.hasAsClause = true;
        }
        
        // 解析类型
        if (typeStr == "@Html") {
            decl.type = ImportType::HTML;
        } else if (typeStr == "@Style") {
            decl.type = ImportType::STYLE;
        } else if (typeStr == "@JavaScript") {
            decl.type = ImportType::JAVASCRIPT;
        } else if (typeStr == "@Chtl") {
            decl.type = ImportType::CHTL;
        } else if (typeStr == "@CJmod") {
            decl.type = ImportType::CJMOD;
        }
        // TODO: 解析其他类型
        
        // 检查通配符
        if (decl.sourcePath.find('*') != std::string::npos) {
            decl.isWildcard = true;
            decl.wildcardPattern = decl.sourcePath;
        }
    }
    
    return decl;
}

bool ImportProcessorEnhanced::validateImportDeclaration(const ImportDeclaration& decl) {
    // 验证路径
    if (decl.sourcePath.empty()) {
        context->reportError("Import path cannot be empty");
        return false;
    }
    
    // 验证类型特定的规则
    switch (decl.type) {
        case ImportType::HTML:
        case ImportType::STYLE:
        case ImportType::JAVASCRIPT:
            // 资源文件不能是文件夹
            if (decl.sourcePath.back() == '/' || decl.sourcePath.back() == '\\') {
                context->reportError("Resource import cannot be a directory: " + decl.sourcePath);
                return false;
            }
            break;
            
        case ImportType::CHTL:
        case ImportType::CJMOD:
            // 模块可以是文件夹（用于通配符）
            break;
            
        default:
            break;
    }
    
    return true;
}

std::string ImportProcessorEnhanced::loadFileContent(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) {
        context->reportError("Cannot open file: " + path.string());
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool ImportProcessorEnhanced::processSubmoduleImport(const std::string& moduleName, 
                                                    const std::string& submodulePath) {
    // TODO: 实现子模块导入逻辑
    // 1. 找到主模块
    // 2. 解析子模块路径
    // 3. 导入指定的子模块
    
    return true;
}

} // namespace chtl