#ifndef CHTL_IMPORT_ENHANCED_H
#define CHTL_IMPORT_ENHANCED_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <mutex>
#include "CHTLImport.h"

namespace chtl {

// 路径规范化器
class PathNormalizer {
public:
    // 规范化路径，处理 . 和 / 的转换，以及相对路径
    static std::filesystem::path normalize(const std::string& path, 
                                          const std::filesystem::path& basePath);
    
    // 判断是否是文件名（不带路径）
    static bool isFileName(const std::string& path);
    
    // 判断是否带有文件扩展名
    static bool hasExtension(const std::string& path);
    
    // 替换路径分隔符（. 转 /）
    static std::string convertDotsToSlashes(const std::string& path);
    
    // 解析通配符模式
    static std::vector<std::string> parseWildcardPattern(const std::string& pattern);
    
    // 检查是否是官方模块引用（chtl::前缀）
    static bool isOfficialModuleReference(const std::string& path);
    
    // 提取官方模块名（去除chtl::前缀）
    static std::string extractOfficialModuleName(const std::string& path);
};

// 增强的文件搜索器
class FileSearcherEnhanced {
private:
    ImportPathConfig config;
    
public:
    FileSearcherEnhanced(const ImportPathConfig& cfg) : config(cfg) {}
    
    // 搜索文件
    std::filesystem::path searchFile(const std::string& name, ImportType type);
    
    // 搜索 HTML/CSS/JS 文件
    std::filesystem::path searchResourceFile(const std::string& name, 
                                            const std::string& extension,
                                            const std::filesystem::path& currentDir);
    
    // 搜索 CHTL/CMOD 文件
    std::filesystem::path searchModuleFile(const std::string& name, bool preferCMOD);
    
    // 搜索官方模块（只在官方目录搜索）
    std::filesystem::path searchOfficialModule(const std::string& name, bool preferCMOD);
    
    // 搜索 CJMOD 文件
    std::filesystem::path searchCJMODFile(const std::string& name);
    
    // 搜索通配符匹配的文件
    std::vector<std::filesystem::path> searchWildcard(const std::string& pattern, 
                                                      const std::filesystem::path& basePath);
};

// 循环依赖检测器实现
class CircularDependencyDetectorImpl : public CircularDependencyDetector {
private:
    // 依赖图：文件 -> 依赖的文件列表
    std::unordered_map<std::string, std::unordered_set<std::string>> dependencyGraph;
    
    // 当前正在处理的文件栈
    std::vector<std::string> processingStack;
    
    // 已完成处理的文件
    std::unordered_set<std::string> processed;
    
    // 线程安全
    mutable std::mutex mutex;
    
public:
    // 开始处理文件
    bool beginProcessing(const std::string& filePath) override;
    
    // 结束处理文件
    void endProcessing(const std::string& filePath) override;
    
    // 添加依赖
    void addDependency(const std::string& from, const std::string& to) override;
    
    // 检测循环依赖
    bool hasCircularDependency(const std::string& from, const std::string& to) const override;
    
    // 获取循环路径
    std::vector<std::string> getCircularPath(const std::string& from, 
                                            const std::string& to) const override;
    
    // 清理
    void clear() override;
    
private:
    // DFS检测循环
    bool dfsDetectCycle(const std::string& current, 
                       const std::string& target,
                       std::unordered_set<std::string>& visited,
                       std::vector<std::string>& path) const;
};

// 增强的导入管理器
class ImportManagerEnhanced : public ImportManager, 
                             public std::enable_shared_from_this<ImportManagerEnhanced> {
private:
    // 路径规范化缓存（避免重复计算）
    mutable std::unordered_map<std::string, std::filesystem::path> pathCache;
    mutable std::mutex pathCacheMutex;
    
    // 文件内容缓存
    std::unordered_map<std::string, std::string> contentCache;
    std::mutex contentCacheMutex;
    
    // 文件搜索器
    std::unique_ptr<FileSearcherEnhanced> fileSearcher;
    
public:
    ImportManagerEnhanced(std::shared_ptr<CHTLContext> ctx);
    
    // 重写处理导入方法
    bool processImport(const ImportDeclaration& decl, 
                      CHTLGenerator& generator);
    
    // 处理资源文件导入（HTML/CSS/JS）
    bool processResourceImport(const ImportDeclaration& decl,
                              std::shared_ptr<CHTLGenerator> generator);
    
    // 处理模块导入（CHTL/CMOD）
    bool processModuleImport(const ImportDeclaration& decl,
                            std::shared_ptr<CHTLGenerator> generator);
    
    // 处理CJMOD导入
    bool processCJMODImport(const ImportDeclaration& decl,
                           std::shared_ptr<CHTLGenerator> generator);
    
    // 处理CMOD压缩包文件
    bool processCMODFile(const std::filesystem::path& cmodPath,
                        std::shared_ptr<CHTLGenerator> generator);
    
    // 处理通配符导入
    bool processWildcardImport(const ImportDeclaration& decl,
                              std::shared_ptr<CHTLGenerator> generator);
    
    // 创建原始嵌入节点（用于带as的资源导入）
    void createOriginNode(const std::string& content, 
                         const std::string& asName,
                         ImportType type,
                         std::shared_ptr<CHTLGenerator> generator);
    
    // 规范化并缓存路径
    std::filesystem::path normalizePath(const std::string& path) const;
    
    // 检查是否已导入（使用规范化路径）
    bool isAlreadyImported(const std::filesystem::path& normalizedPath) const;
    
    // 标记为已导入（使用规范化路径）
    void markAsImported(const std::filesystem::path& normalizedPath);
};

// 增强的导入处理器（处理具体的导入逻辑）
class ImportProcessorEnhanced {
private:
    std::shared_ptr<ImportManagerEnhanced> manager;
    std::shared_ptr<CHTLContext> context;
    
public:
    ImportProcessorEnhanced(std::shared_ptr<ImportManagerEnhanced> mgr,
                           std::shared_ptr<CHTLContext> ctx)
        : manager(mgr), context(ctx) {}
    
    // 解析导入语句
    ImportDeclaration parseImportStatement(const std::string& statement);
    
    // 验证导入声明
    bool validateImportDeclaration(const ImportDeclaration& decl);
    
    // 加载文件内容
    std::string loadFileContent(const std::filesystem::path& path);
    
    // 处理子模块导入
    bool processSubmoduleImport(const std::string& moduleName, 
                               const std::string& submodulePath);
    
    // 处理模块内容
    bool processModuleContent(const std::string& content,
                             std::shared_ptr<CHTLGenerator> generator);
};

} // namespace chtl

#endif // CHTL_IMPORT_ENHANCED_H