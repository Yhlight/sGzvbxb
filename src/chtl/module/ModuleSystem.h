#ifndef CHTL_MODULE_SYSTEM_H
#define CHTL_MODULE_SYSTEM_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <set>
#include <filesystem>
#include "../scanner/CHTLUnifiedScanner.h"
#include "../compiler/CompilerInterface.h"

namespace chtl {
// 前向声明
class CHTLContext;

namespace module {

// 模块类型
enum class ModuleType {
    CHTL,       // .chtl文件
    CMOD,       // CMOD模块
    CJMOD,      // CJMOD模块
    HTML,       // 外部HTML
    CSS,        // 外部CSS
    JavaScript  // 外部JS
};

// 导入项类型
enum class ImportItemType {
    Element,    // @Element
    Style,      // @Style
    Var,        // @Var
    Html,       // @Html
    JavaScript, // @JavaScript
    Chtl,       // @Chtl (整个文件)
    CJmod       // @CJmod
};

// 导入项信息
struct ImportItem {
    ImportItemType type;
    std::string itemName;       // 具体项名称（如Button）
    std::string sourcePath;     // 来源文件路径
    std::string alias;          // as别名（可选）
    bool isTemplate;            // 是否是[Template]
    bool isCustom;              // 是否是[Custom]
    
    std::string getFullName() const {
        return alias.empty() ? itemName : alias;
    }
};

// 导出项信息
struct ExportItem {
    ImportItemType type;
    std::string itemName;
    bool isTemplate;
    bool isCustom;
};

// 模块信息
class ModuleInfo {
public:
    ModuleInfo(const std::string& path, ModuleType type);
    
    // 基本信息
    const std::string& getPath() const { return path_; }
    ModuleType getType() const { return type_; }
    const std::string& getNamespace() const { return namespace_; }
    void setNamespace(const std::string& ns) { namespace_ = ns; }
    
    // 导入导出管理
    void addImport(const ImportItem& item);
    void addExport(const ExportItem& item);
    const std::vector<ImportItem>& getImports() const { return imports_; }
    const std::vector<ExportItem>& getExports() const { return exports_; }
    
    // 依赖管理
    void addDependency(const std::string& path) { dependencies_.insert(path); }
    const std::set<std::string>& getDependencies() const { return dependencies_; }
    
    // 缓存管理
    void setCachedContent(const std::string& content) { cachedContent_ = content; }
    const std::string& getCachedContent() const { return cachedContent_; }
    bool isCached() const { return !cachedContent_.empty(); }
    
private:
    std::string path_;
    ModuleType type_;
    std::string namespace_;
    std::vector<ImportItem> imports_;
    std::vector<ExportItem> exports_;
    std::set<std::string> dependencies_;
    std::string cachedContent_;
};

// 模块解析器
class ModuleParser {
public:
    ModuleParser();
    
    // 解析模块文件
    std::shared_ptr<ModuleInfo> parseModule(const std::string& path);
    
private:
    // 解析不同类型的导入语句
    ImportItem parseImportStatement(const std::string& statement);
    
    // 解析导出块
    std::vector<ExportItem> parseExportBlock(const std::string& block);
    
    // 提取命名空间
    std::string extractNamespace(const std::string& content);
    
    // 判断文件类型
    ModuleType detectModuleType(const std::string& path);
};

// 模块加载器
class ModuleLoader {
public:
    ModuleLoader();
    
    // 设置模块搜索路径
    void addSearchPath(const std::string& path);
    
    // 加载模块
    std::shared_ptr<ModuleInfo> loadModule(const std::string& path);
    
    // 解析模块路径（支持.表示/）
    std::string resolvePath(const std::string& importPath, const std::string& currentFile);
    
private:
    std::vector<std::string> searchPaths_;
    std::unordered_map<std::string, std::shared_ptr<ModuleInfo>> moduleCache_;
    ModuleParser parser_;
    
    // 查找模块文件
    std::string findModuleFile(const std::string& path);
    
    // 读取文件内容
    std::string readFile(const std::string& path);
};

// 模块依赖图
class ModuleDependencyGraph {
public:
    ModuleDependencyGraph();
    
    // 添加模块
    void addModule(std::shared_ptr<ModuleInfo> module);
    
    // 检测循环依赖
    bool hasCircularDependency() const;
    std::vector<std::string> findCircularPath() const;
    
    // 拓扑排序（用于确定编译顺序）
    std::vector<std::string> topologicalSort() const;
    
    // 获取模块的所有依赖（递归）
    std::set<std::string> getAllDependencies(const std::string& modulePath) const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<ModuleInfo>> modules_;
    std::unordered_map<std::string, std::set<std::string>> adjacencyList_;
    
    // DFS辅助函数
    bool dfsHasCycle(const std::string& node, 
                     std::set<std::string>& visited,
                     std::set<std::string>& recursionStack,
                     std::vector<std::string>& path) const;
};

// 模块管理器 - 主接口
class ModuleManager {
public:
    ModuleManager();
    
    // 初始化
    void initialize(const std::string& projectRoot);
    
    // 加载项目的所有模块
    void loadProject();
    
    // 获取模块信息
    std::shared_ptr<ModuleInfo> getModule(const std::string& path);
    
    // 解析导入
    std::vector<ImportItem> resolveImports(const std::string& fromModule);
    
    // 获取编译顺序
    std::vector<std::string> getCompilationOrder();
    
    // 检查循环依赖
    bool checkDependencies();
    
    // CMOD/CJMOD支持
    bool isCMOD(const std::string& path) const;
    bool isCJMOD(const std::string& path) const;
    std::vector<std::string> getCMODFiles(const std::string& moduleName) const;
    std::vector<std::string> getCJMODFiles(const std::string& moduleName) const;
    
private:
    std::string projectRoot_;
    ModuleLoader loader_;
    ModuleDependencyGraph dependencyGraph_;
    
    // 扫描项目目录
    void scanDirectory(const std::string& dir);
    
    // 处理CMOD/CJMOD目录结构
    void processCMODDirectory(const std::string& dir);
    void processCJMODDirectory(const std::string& dir);
};

// 模块编译器 - 处理模块的编译
class ModuleCompiler {
public:
    ModuleCompiler(std::shared_ptr<CHTLContext> context);
    
    // 编译单个模块
    compiler::CompileResult compileModule(std::shared_ptr<ModuleInfo> module);
    
    // 编译模块列表（按依赖顺序）
    std::vector<compiler::CompileResult> compileModules(
        const std::vector<std::shared_ptr<ModuleInfo>>& modules
    );
    
    // 增量编译
    std::vector<compiler::CompileResult> incrementalCompile(
        const std::set<std::string>& changedFiles,
        ModuleManager& moduleManager
    );
    
private:
    std::shared_ptr<CHTLContext> context_;
    std::unordered_map<std::string, std::string> compiledCache_;
    
    // 合并编译结果
    std::string mergeResults(const std::vector<compiler::CompileResult>& results);
    
    // 处理导入替换
    std::string processImports(const std::string& content, 
                              const std::vector<ImportItem>& imports);
};

} // namespace module
} // namespace chtl

#endif // CHTL_MODULE_SYSTEM_H