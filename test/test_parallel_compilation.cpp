#include "../src/chtl/compiler/CompilerDispatcher.h"
#include "../src/chtl/optimization/ParallelCompiler.h"
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace chtl;

void testParallelCompilation() {
    std::cout << "=== Parallel Compilation Test ===" << std::endl;
    
    // 创建一个包含多个独立片段的复杂CHTL源代码
    std::string testSource = R"(
// 全局样式
style {
    body {
        margin: 0;
        padding: 0;
        font-family: Arial, sans-serif;
    }
    
    .container {
        max-width: 1200px;
        margin: 0 auto;
    }
    
    .header {
        background: #333;
        color: white;
        padding: 1rem;
    }
}

// HTML结构
body {
    header {
        class: "header";
        h1 { text "Parallel Compilation Demo" }
    }
    
    div {
        class: "container";
        
        // 多个独立的组件
        div {
            class: "component-1";
            style {
                background: #f0f0f0;
                padding: 20px;
            }
            h2 { text "Component 1" }
            p { text "This is component 1" }
        }
        
        div {
            class: "component-2";
            style {
                background: #e0e0e0;
                padding: 20px;
            }
            h2 { text "Component 2" }
            p { text "This is component 2" }
        }
        
        div {
            class: "component-3";
            style {
                background: #d0d0d0;
                padding: 20px;
            }
            h2 { text "Component 3" }
            p { text "This is component 3" }
        }
    }
    
    // 多个独立的脚本块
    script {
        console.log("Script block 1");
        function init1() {
            console.log("Initializing component 1");
        }
    }
    
    script {
        console.log("Script block 2");
        function init2() {
            console.log("Initializing component 2");
        }
    }
    
    script {
        // CHTL JS 功能
        {{.component-1}}.listen({
            click: () => {
                console.log("Component 1 clicked");
            }
        });
        
        {{.component-2}}.animate({
            opacity: [0, 1]
        }, {
            duration: 500
        });
    }
}

// 更多独立的CSS
style {
    .button {
        background: #007bff;
        color: white;
        padding: 10px 20px;
        border: none;
        border-radius: 4px;
        cursor: pointer;
    }
    
    .button:hover {
        background: #0056b3;
    }
}
)";
    
    compiler::CompilerDispatcher sequentialCompiler;
    compiler::CompilerDispatcher parallelCompiler;
    
    // 配置并行编译器
    parallelCompiler.setParallelCompilation(true);
    
    // 预热
    sequentialCompiler.compile("body { text \"warmup\" }");
    parallelCompiler.compile("body { text \"warmup\" }");
    
    // 测试顺序编译
    auto start = std::chrono::high_resolution_clock::now();
    auto seqResult = sequentialCompiler.compile(testSource);
    auto end = std::chrono::high_resolution_clock::now();
    auto seqDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "Sequential compilation: " << seqDuration << " ms" << std::endl;
    std::cout << "  Success: " << (seqResult.success ? "Yes" : "No") << std::endl;
    
    // 测试并行编译
    start = std::chrono::high_resolution_clock::now();
    auto parResult = parallelCompiler.compile(testSource);
    end = std::chrono::high_resolution_clock::now();
    auto parDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "\nParallel compilation: " << parDuration << " ms" << std::endl;
    std::cout << "  Success: " << (parResult.success ? "Yes" : "No") << std::endl;
    
    // 计算加速比
    if (parDuration > 0) {
        double speedup = static_cast<double>(seqDuration) / parDuration;
        std::cout << "\nSpeedup: " << std::fixed << std::setprecision(2) << speedup << "x" << std::endl;
    }
    
    // 显示并行编译统计
    auto& parallelCompilerImpl = optimization::ParallelCompilerManager::getInstance().getCompiler();
    auto stats = parallelCompilerImpl.getStats();
    
    std::cout << "\nParallel Compilation Statistics:" << std::endl;
    std::cout << "  Total fragments: " << stats.totalFragments << std::endl;
    std::cout << "  Parallelized fragments: " << stats.parallelizedFragments << std::endl;
    std::cout << "  Hardware threads: " << std::thread::hardware_concurrency() << std::endl;
}

void testThreadPoolPerformance() {
    std::cout << "\n=== Thread Pool Performance Test ===" << std::endl;
    
    // 测试不同线程数的性能
    std::vector<size_t> threadCounts = {1, 2, 4, 8, std::thread::hardware_concurrency()};
    
    std::string testSource = R"(
style { body { margin: 0; } }
body { div { text "Test" } }
)";
    
    for (size_t threads : threadCounts) {
        // 配置线程数
        optimization::ParallelCompilerManager::getInstance().configure(threads, false);
        
        compiler::CompilerDispatcher compiler;
        compiler.setParallelCompilation(true);
        
        // 运行多次取平均
        const int runs = 10;
        long totalTime = 0;
        
        for (int i = 0; i < runs; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            compiler.compile(testSource);
            auto end = std::chrono::high_resolution_clock::now();
            totalTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        }
        
        double avgTime = static_cast<double>(totalTime) / runs;
        std::cout << "  Threads: " << threads << ", Avg time: " 
                  << std::fixed << std::setprecision(2) << avgTime << " µs" << std::endl;
    }
}

int main() {
    std::cout << "CHTL Parallel Compilation Test Suite\n" << std::endl;
    
    testParallelCompilation();
    testThreadPoolPerformance();
    
    std::cout << "\nAll tests completed!" << std::endl;
    
    return 0;
}