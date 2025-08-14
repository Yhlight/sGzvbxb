#ifndef CHTL_PARALLEL_COMPILER_H
#define CHTL_PARALLEL_COMPILER_H

#include <vector>
#include <string>
#include <future>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <set>
#include <unordered_map>
#include "../compiler/CompilerInterface.h"
#include "../scanner/CHTLUnifiedScanner.h"
#include "../compiler/CompilerDispatcher.h"

namespace chtl {
namespace parallel {

// 编译任务
struct CompileTask {
    std::string filepath;
    std::string content;
    size_t priority;  // 优先级，越小越高
    
    bool operator<(const CompileTask& other) const {
        return priority > other.priority;  // 优先级队列需要反向比较
    }
};

// 编译结果
struct CompileResult {
    std::string filepath;
    bool success;
    std::string output;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::chrono::milliseconds duration;
};

// 线程池
class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();
    
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    
    void stop();
    
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_;
};

// 并行编译器
class ParallelCompiler {
public:
    struct Options {
        size_t maxThreads = std::thread::hardware_concurrency();
        bool enableCache = true;
        bool enableDependencyAnalysis = true;
        size_t batchSize = 10;  // 每批处理的文件数
    };
    
    ParallelCompiler(const Options& options = Options());
    ~ParallelCompiler();
    
    // 编译单个文件
    std::future<CompileResult> compileFileAsync(const std::string& filepath);
    
    // 批量编译
    std::vector<CompileResult> compileFiles(const std::vector<std::string>& filepaths);
    
    // 编译目录
    std::vector<CompileResult> compileDirectory(
        const std::string& directory, 
        bool recursive = true
    );
    
    // 增量编译
    std::vector<CompileResult> incrementalCompile(
        const std::vector<std::string>& changedFiles
    );
    
    // 获取编译进度
    struct Progress {
        size_t total;
        size_t completed;
        size_t failed;
        double percentage() const {
            return total > 0 ? static_cast<double>(completed) / total * 100.0 : 0.0;
        }
    };
    
    Progress getProgress() const;
    
    // 取消编译
    void cancel();
    
private:
    Options options_;
    std::unique_ptr<ThreadPool> threadPool_;
    std::unique_ptr<scanner::CHTLUnifiedScanner> scanner_;
    std::unique_ptr<compiler::CompilerDispatcher> dispatcher_;
    
    // 进度跟踪
    mutable std::mutex progressMutex_;
    size_t totalTasks_;
    size_t completedTasks_;
    size_t failedTasks_;
    
    // 依赖分析
    std::unordered_map<std::string, std::set<std::string>> dependencies_;
    void analyzeDependencies(const std::string& filepath);
    std::vector<std::string> topologicalSort(const std::vector<std::string>& files);
    
    // 编译实现
    CompileResult compileFileImpl(const std::string& filepath);
    
    // 文件扫描
    std::vector<std::string> scanDirectory(const std::string& directory, bool recursive);
    
    // 缓存集成
    bool tryGetFromCache(const std::string& filepath, CompileResult& result);
    void putToCache(const std::string& filepath, const CompileResult& result);
};

// 编译管道 - 支持流式处理
class CompilePipeline {
public:
    CompilePipeline();
    
    // 添加处理阶段
    CompilePipeline& addStage(
        const std::string& name,
        std::function<void(scanner::CodeFragment&)> processor
    );
    
    // 设置并行度
    CompilePipeline& setParallelism(size_t threads);
    
    // 执行管道
    std::vector<CompileResult> execute(const std::vector<std::string>& files);
    
private:
    struct Stage {
        std::string name;
        std::function<void(scanner::CodeFragment&)> processor;
    };
    
    std::vector<Stage> stages_;
    size_t parallelism_;
};

} // namespace parallel
} // namespace chtl

#endif // CHTL_PARALLEL_COMPILER_H