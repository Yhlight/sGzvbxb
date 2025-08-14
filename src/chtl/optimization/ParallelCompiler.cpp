#include "ParallelCompiler.h"
#include <algorithm>
#include <numeric>
#include <iostream>

namespace chtl {
namespace optimization {

// ThreadPool 实现
ThreadPool::ThreadPool(size_t numThreads) : stop_(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back([this] {
            for (;;) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(this->queueMutex_);
                    this->condition_.wait(lock, [this] { 
                        return this->stop_ || !this->tasks_.empty(); 
                    });
                    
                    if (this->stop_ && this->tasks_.empty()) {
                        return;
                    }
                    
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }
                
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        stop_ = true;
    }
    
    condition_.notify_all();
    
    for (std::thread &worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

template<typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type> {
    
    using return_type = typename std::result_of<F(Args...)>::type;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<return_type> res = task->get_future();
    
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        
        if (stop_) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        
        tasks_.emplace([task](){ (*task)(); });
    }
    
    condition_.notify_one();
    return res;
}

// ParallelCompiler thread_local 静态成员定义
thread_local std::unique_ptr<compiler::CompilerDispatcher> ParallelCompiler::tlsCompiler_;

// ParallelCompiler 实现
ParallelCompiler::ParallelCompiler(size_t numThreads) 
    : maxThreads_(numThreads == 0 ? std::thread::hardware_concurrency() : numThreads)
    , enableParallel_(true)
    , batchSize_(10) {
    
    threadPool_ = std::make_unique<ThreadPool>(maxThreads_);
}

ParallelCompiler::~ParallelCompiler() = default;

compiler::CompilationResult ParallelCompiler::compileParallel(
    const std::vector<scanner::CodeFragment>& fragments) {
    
    auto startTime = std::chrono::steady_clock::now();
    
    if (!enableParallel_ || fragments.size() < 2) {
        // 使用顺序编译
        compiler::CompilerDispatcher dispatcher;
        // 需要修改CompilerDispatcher以支持直接编译片段
        // 这里暂时返回空结果
        compiler::CompilationResult result;
        result.success = true;
        return result;
    }
    
    // 将片段分组以减少依赖
    auto groups = groupFragments(fragments);
    
    std::vector<std::future<ParallelCompileResult>> futures;
    std::vector<ParallelCompileResult> results;
    
    // 为每组创建编译任务
    size_t taskId = 0;
    for (const auto& group : groups) {
        for (const auto& task : group) {
            futures.push_back(
                threadPool_->enqueue([this, task]() {
                    return this->compileFragment(task);
                })
            );
        }
        
        // 等待当前组完成（保持依赖顺序）
        for (auto& future : futures) {
            results.push_back(future.get());
        }
        futures.clear();
    }
    
    auto endTime = std::chrono::steady_clock::now();
    
    // 更新统计信息
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        stats_.totalFragments = fragments.size();
        stats_.parallelizedFragments = results.size();
        stats_.totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime
        );
    }
    
    // 合并结果
    return mergeResults(results);
}

void ParallelCompiler::setMaxThreads(size_t threads) {
    maxThreads_ = threads;
    threadPool_ = std::make_unique<ThreadPool>(maxThreads_);
}

void ParallelCompiler::setEnableParallel(bool enable) {
    enableParallel_ = enable;
}

void ParallelCompiler::setBatchSize(size_t size) {
    batchSize_ = size;
}

ParallelCompiler::PerformanceStats ParallelCompiler::getStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

bool ParallelCompiler::canParallelize(const scanner::CodeFragment& fragment) const {
    // CSS和独立的JavaScript片段可以并行编译
    // CHTL片段由于可能有依赖关系，需要更谨慎
    switch (fragment.type) {
        case scanner::FragmentType::CSS:
        case scanner::FragmentType::JAVASCRIPT:
            return true;
        case scanner::FragmentType::CHTL:
        case scanner::FragmentType::CHTL_JS:
            // 简单的启发式：如果片段不包含import或template定义，可以并行
            return fragment.content.find("[Import]") == std::string::npos &&
                   fragment.content.find("[Template]") == std::string::npos;
        default:
            return false;
    }
}

ParallelCompileResult ParallelCompiler::compileFragment(const CompileTask& task) {
    ParallelCompileResult result;
    result.id = task.id;
    
    try {
        // 获取线程本地编译器实例
        if (!tlsCompiler_) {
            tlsCompiler_ = std::make_unique<compiler::CompilerDispatcher>();
        }
        
        // 这里需要CompilerDispatcher支持编译单个片段
        // 暂时返回成功
        result.success = true;
        result.result.type = task.type;
        
        switch (task.type) {
            case scanner::FragmentType::CSS:
                result.result.output = task.fragment.content;
                break;
            case scanner::FragmentType::JAVASCRIPT:
                result.result.output = task.fragment.content;
                break;
            case scanner::FragmentType::CHTL:
                result.result.output = "<div>Compiled CHTL</div>";
                break;
            case scanner::FragmentType::CHTL_JS:
                result.result.output = "// Compiled CHTL JS\n" + task.fragment.content;
                break;
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error = e.what();
    }
    
    return result;
}

compiler::CompilationResult ParallelCompiler::mergeResults(
    const std::vector<ParallelCompileResult>& results) {
    
    compiler::CompilationResult merged;
    merged.success = true;
    
    std::stringstream htmlStream, cssStream, jsStream;
    
    for (const auto& result : results) {
        if (!result.success) {
            merged.success = false;
            merged.errors.push_back({result.error, 0, 0});
            continue;
        }
        
        if (!result.result.output.empty()) {
            // 根据类型分发输出
            switch (result.result.type) {
                case scanner::FragmentType::CHTL:
                    htmlStream << result.result.output;
                    break;
                case scanner::FragmentType::CSS:
                    cssStream << result.result.output << "\n";
                    break;
                case scanner::FragmentType::JAVASCRIPT:
                case scanner::FragmentType::CHTL_JS:
                    jsStream << result.result.output << "\n";
                    break;
            }
        }
    }
    
    merged.html = htmlStream.str();
    merged.css = cssStream.str();
    merged.javascript = jsStream.str();
    
    return merged;
}

std::vector<std::vector<CompileTask>> ParallelCompiler::groupFragments(
    const std::vector<scanner::CodeFragment>& fragments) {
    
    std::vector<std::vector<CompileTask>> groups;
    std::vector<CompileTask> currentGroup;
    
    size_t taskId = 0;
    for (const auto& fragment : fragments) {
        CompileTask task;
        task.id = std::to_string(taskId++);
        task.fragment = fragment;
        task.type = fragment.type;
        
        if (canParallelize(fragment)) {
            currentGroup.push_back(task);
            
            // 批量大小限制
            if (currentGroup.size() >= batchSize_) {
                groups.push_back(std::move(currentGroup));
                currentGroup.clear();
            }
        } else {
            // 不能并行的片段单独成组
            if (!currentGroup.empty()) {
                groups.push_back(std::move(currentGroup));
                currentGroup.clear();
            }
            groups.push_back({task});
        }
    }
    
    if (!currentGroup.empty()) {
        groups.push_back(std::move(currentGroup));
    }
    
    return groups;
}

// ParallelCompilerManager 实现
ParallelCompilerManager& ParallelCompilerManager::getInstance() {
    static ParallelCompilerManager instance;
    return instance;
}

ParallelCompilerManager::ParallelCompilerManager()
    : compiler_(std::make_unique<ParallelCompiler>())
    , enabled_(true) {
}

ParallelCompiler& ParallelCompilerManager::getCompiler() {
    return *compiler_;
}

void ParallelCompilerManager::configure(size_t maxThreads, bool autoDetect) {
    if (autoDetect && maxThreads == 0) {
        maxThreads = std::thread::hardware_concurrency();
    }
    compiler_->setMaxThreads(maxThreads);
}

void ParallelCompilerManager::setEnabled(bool enabled) {
    enabled_ = enabled;
    compiler_->setEnableParallel(enabled);
}

bool ParallelCompilerManager::isEnabled() const {
    return enabled_;
}

} // namespace optimization
} // namespace chtl