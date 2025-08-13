#ifndef CHTL_PERFORMANCE_OPTIMIZER_H
#define CHTL_PERFORMANCE_OPTIMIZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <set>
#include <optional>
#include "../scanner/CHTLUnifiedScanner.h"
#include "../cache/FragmentCache.h"

namespace chtl {
namespace optimization {

// 性能分析数据
struct PerformanceMetrics {
    std::chrono::milliseconds scanTime;
    std::chrono::milliseconds parseTime;
    std::chrono::milliseconds compileTime;
    std::chrono::milliseconds optimizeTime;
    std::chrono::milliseconds totalTime;
    
    size_t fragmentCount;
    size_t cacheHits;
    size_t cacheMisses;
    
    double cacheHitRate() const {
        size_t total = cacheHits + cacheMisses;
        return total > 0 ? static_cast<double>(cacheHits) / total : 0.0;
    }
};

// AST优化器
class ASTOptimizer {
public:
    ASTOptimizer();
    
    // 优化AST
    void optimize(void* ast);  // 实际类型取决于具体的AST实现
    
    // 死代码消除
    void eliminateDeadCode(void* ast);
    
    // 常量折叠
    void foldConstants(void* ast);
    
    // 内联优化
    void inlineOptimization(void* ast);
    
private:
    // 检测未使用的变量/函数
    std::set<std::string> findUnusedSymbols(void* ast);
    
    // 计算常量表达式
    std::optional<std::string> evaluateConstExpr(void* expr);
};

// CSS优化器
class CSSOptimizer {
public:
    CSSOptimizer();
    
    // 优化CSS内容
    std::string optimize(const std::string& css);
    
    // 合并相同选择器
    std::string mergeSelectors(const std::string& css);
    
    // 压缩CSS
    std::string minify(const std::string& css);
    
    // 删除未使用的CSS规则
    std::string removeUnusedRules(const std::string& css, 
                                 const std::set<std::string>& usedSelectors);
    
    // 优化选择器性能
    std::string optimizeSelectors(const std::string& css);
    
private:
    struct CSSRule {
        std::string selector;
        std::string properties;
        int specificity;
    };
    
    // 解析CSS规则
    std::vector<CSSRule> parseCSS(const std::string& css);
    
    // 计算选择器优先级
    int calculateSpecificity(const std::string& selector);
    
    // 合并属性
    std::string mergeProperties(const std::vector<std::string>& props);
};

// JavaScript优化器
class JSOptimizer {
public:
    JSOptimizer();
    
    // 优化JavaScript代码
    std::string optimize(const std::string& js);
    
    // 压缩JS
    std::string minify(const std::string& js);
    
    // 删除死代码
    std::string removeDeadCode(const std::string& js);
    
    // 优化CHTL JS特性
    std::string optimizeCHTLJS(const std::string& js);
    
private:
    // 分析变量使用
    std::set<std::string> analyzeVariableUsage(const std::string& js);
    
    // 优化选择器缓存
    std::string cacheDOMSelectors(const std::string& js);
    
    // 优化事件监听
    std::string optimizeEventListeners(const std::string& js);
};

// 增量编译优化器
class IncrementalCompiler {
public:
    IncrementalCompiler();
    
    // 初始化项目状态
    void initialize(const std::string& projectPath);
    
    // 检测变更文件
    std::vector<std::string> detectChangedFiles();
    
    // 分析影响范围
    std::set<std::string> analyzeImpact(const std::vector<std::string>& changedFiles);
    
    // 增量编译
    void incrementalCompile(const std::set<std::string>& affectedFiles);
    
private:
    struct FileInfo {
        std::string path;
        std::chrono::system_clock::time_point lastModified;
        size_t contentHash;
        std::set<std::string> dependencies;
        std::set<std::string> dependents;
    };
    
    std::unordered_map<std::string, FileInfo> fileInfoMap_;
    std::string projectPath_;
    
    // 计算文件哈希
    size_t computeFileHash(const std::string& path);
    
    // 构建依赖图
    void buildDependencyGraph();
};

// 内存优化器
class MemoryOptimizer {
public:
    MemoryOptimizer();
    
    // 优化内存使用
    void optimizeMemoryUsage();
    
    // 设置内存限制
    void setMemoryLimit(size_t bytes);
    
    // 获取内存使用情况
    struct MemoryStats {
        size_t totalAllocated;
        size_t peakUsage;
        size_t currentUsage;
        size_t fragmentCacheSize;
        size_t astNodeCount;
    };
    
    MemoryStats getMemoryStats() const;
    
    // 触发垃圾回收
    void collectGarbage();
    
private:
    size_t memoryLimit_;
    MemoryStats stats_;
    
    // 释放未使用的缓存
    void releaseUnusedCache();
    
    // 压缩AST节点
    void compactASTNodes();
};

// 并行优化调度器
class ParallelOptimizer {
public:
    ParallelOptimizer(size_t threadCount = 0);  // 0表示自动检测
    
    // 并行优化多个文件
    std::vector<std::string> optimizeFiles(const std::vector<std::string>& files);
    
    // 设置优化级别
    enum class OptimizationLevel {
        None = 0,
        Basic = 1,      // 基础优化
        Standard = 2,   // 标准优化
        Aggressive = 3  // 激进优化
    };
    
    void setOptimizationLevel(OptimizationLevel level);
    
private:
    size_t threadCount_;
    OptimizationLevel optimizationLevel_;
    
    // 分配任务到线程
    std::vector<std::vector<std::string>> distributeWork(
        const std::vector<std::string>& files
    );
};

// 主性能优化器
class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    
    // 启用/禁用特定优化
    void enableOptimization(const std::string& name, bool enable = true);
    
    // 优化编译流程
    void optimizeCompilation(const std::string& input, std::string& output);
    
    // 获取性能指标
    PerformanceMetrics getMetrics() const;
    
    // 性能分析报告
    std::string generatePerformanceReport() const;
    
    // 配置优化选项
    struct Options {
        bool enableCaching = true;
        bool enableParallel = true;
        bool enableIncremental = true;
        bool enableDeadCodeElimination = true;
        bool enableMinification = true;
        bool enableSelectorOptimization = true;
        size_t maxMemoryUsage = 500 * 1024 * 1024;  // 500MB
        size_t maxThreads = 0;  // 0 = auto
    };
    
    void setOptions(const Options& options);
    
private:
    Options options_;
    PerformanceMetrics metrics_;
    
    std::unique_ptr<ASTOptimizer> astOptimizer_;
    std::unique_ptr<CSSOptimizer> cssOptimizer_;
    std::unique_ptr<JSOptimizer> jsOptimizer_;
    std::unique_ptr<IncrementalCompiler> incrementalCompiler_;
    std::unique_ptr<MemoryOptimizer> memoryOptimizer_;
    std::unique_ptr<ParallelOptimizer> parallelOptimizer_;
    
    // 性能计时器
    template<typename F>
    auto measureTime(F&& func, std::chrono::milliseconds& duration) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = func();
        auto end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return result;
    }
};

} // namespace optimization
} // namespace chtl

#endif // CHTL_PERFORMANCE_OPTIMIZER_H