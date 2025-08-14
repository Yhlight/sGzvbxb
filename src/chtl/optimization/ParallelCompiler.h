#ifndef CHTL_PARALLEL_COMPILER_H
#define CHTL_PARALLEL_COMPILER_H

#include <vector>
#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include "../scanner/CHTLUnifiedScanner.h"
#include "../compiler/CompilerDispatcher.h"

namespace chtl {
namespace optimization {

// 并行编译任务
struct CompileTask {
    std::string id;
    scanner::CodeFragment fragment;
    scanner::FragmentType type;
};

// 并行编译结果
struct ParallelCompileResult {
    std::string id;
    compiler::FragmentResult result;
    bool success;
    std::string error;
};

// 线程池
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();
    
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    
    void shutdown();
    size_t getThreadCount() const { return workers_.size(); }
    
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
};

// 并行编译器
class ParallelCompiler {
public:
    explicit ParallelCompiler(size_t numThreads = 0);
    ~ParallelCompiler();
    
    // 并行编译多个片段
    compiler::CompilationResult compileParallel(const std::vector<scanner::CodeFragment>& fragments);
    
    // 配置选项
    void setMaxThreads(size_t threads);
    void setEnableParallel(bool enable);
    void setBatchSize(size_t size);
    
    // 性能统计
    struct PerformanceStats {
        size_t totalFragments;
        size_t parallelizedFragments;
        double speedup;
        std::chrono::milliseconds totalTime;
        std::chrono::milliseconds parallelTime;
        std::chrono::milliseconds sequentialTime;
    };
    
    PerformanceStats getStats() const;
    
private:
    // 判断片段是否可以并行编译
    bool canParallelize(const scanner::CodeFragment& fragment) const;
    
    // 编译单个片段
    ParallelCompileResult compileFragment(const CompileTask& task);
    
    // 合并并行编译结果
    compiler::CompilationResult mergeResults(const std::vector<ParallelCompileResult>& results);
    
    // 优化片段分组（减少依赖）
    std::vector<std::vector<CompileTask>> groupFragments(const std::vector<scanner::CodeFragment>& fragments);

private:
    std::unique_ptr<ThreadPool> threadPool_;
    size_t maxThreads_;
    bool enableParallel_;
    size_t batchSize_;
    
    // 编译器实例（每个线程一个）
    thread_local static std::unique_ptr<compiler::CompilerDispatcher> tlsCompiler_;
    
    // 性能统计
    mutable PerformanceStats stats_;
    mutable std::mutex statsMutex_;
};

// 全局并行编译管理器
class ParallelCompilerManager {
public:
    static ParallelCompilerManager& getInstance();
    
    ParallelCompiler& getCompiler();
    
    // 配置全局并行编译选项
    void configure(size_t maxThreads, bool autoDetect = true);
    
    // 启用/禁用并行编译
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
private:
    ParallelCompilerManager();
    std::unique_ptr<ParallelCompiler> compiler_;
    bool enabled_;
};

} // namespace optimization
} // namespace chtl

#endif // CHTL_PARALLEL_COMPILER_H