#include "../src/chtl/cache/FragmentCache.h"
#include "../src/chtl/compiler/CompilerDispatcher.h"
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace chtl;

void testCachePerformance() {
    std::cout << "=== Cache Performance Test ===" << std::endl;
    
    // 准备测试数据
    std::string testSource = R"(
style {
    body {
        margin: 0;
        padding: 0;
    }
}

body {
    h1 {
        text "Hello CHTL"
    }
    
    script {
        {{.btn}}.listen({
            click: () => {
                console.log("Clicked");
            }
        });
    }
}
)";
    
    compiler::CompilerDispatcher dispatcher;
    auto& cache = cache::FragmentCacheManager::getInstance().getCache();
    
    // 清空缓存
    cache.clear();
    
    // 第一次编译（无缓存）
    auto start = std::chrono::high_resolution_clock::now();
    auto result1 = dispatcher.compile(testSource);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << "First compilation (no cache): " << duration1 << " µs" << std::endl;
    
    // 第二次编译（有缓存）
    start = std::chrono::high_resolution_clock::now();
    auto result2 = dispatcher.compile(testSource);
    end = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << "Second compilation (with cache): " << duration2 << " µs" << std::endl;
    std::cout << "Speedup: " << std::fixed << std::setprecision(2) 
              << static_cast<double>(duration1) / duration2 << "x" << std::endl;
    
    // 显示缓存统计
    auto stats = cache.getStats();
    std::cout << "\nCache Statistics:" << std::endl;
    std::cout << "  Hit rate: " << stats.hitRate() * 100 << "%" << std::endl;
    std::cout << "  Total hits: " << stats.totalHits << std::endl;
    std::cout << "  Total misses: " << stats.totalMisses << std::endl;
    std::cout << "  Current size: " << stats.currentSize << " entries" << std::endl;
}

void testCachePersistence() {
    std::cout << "\n=== Cache Persistence Test ===" << std::endl;
    
    auto& cache = cache::FragmentCacheManager::getInstance().getCache();
    
    // 保存到磁盘
    std::string cacheFile = "/tmp/chtl_cache.bin";
    if (cache.saveToDisk(cacheFile)) {
        std::cout << "Cache saved to " << cacheFile << std::endl;
        
        // 清空内存缓存
        cache.clear();
        
        // 从磁盘加载
        if (cache.loadFromDisk(cacheFile)) {
            std::cout << "Cache loaded from disk" << std::endl;
            
            auto stats = cache.getStats();
            std::cout << "Loaded " << stats.currentSize << " entries" << std::endl;
        } else {
            std::cerr << "Failed to load cache from disk" << std::endl;
        }
    } else {
        std::cerr << "Failed to save cache to disk" << std::endl;
    }
}

void testCacheEviction() {
    std::cout << "\n=== Cache Eviction Test ===" << std::endl;
    
    auto& cache = cache::FragmentCacheManager::getInstance().getCache();
    cache.clear();
    
    // 设置小缓存大小以触发淘汰
    cache.setMaxCacheSize(1024); // 1KB
    
    // 添加多个条目
    scanner::CHTLUnifiedScanner scanner;
    
    for (int i = 0; i < 10; i++) {
        std::string source = "body { text \"Test " + std::to_string(i) + "\" }";
        auto fragments = scanner.scan(source);
        cache.put(source, fragments);
    }
    
    auto stats = cache.getStats();
    std::cout << "After adding 10 entries:" << std::endl;
    std::cout << "  Current size: " << stats.currentSize << " entries" << std::endl;
    std::cout << "  Total evictions: " << stats.totalEvictions << std::endl;
    
    // 恢复正常缓存大小
    cache.setMaxCacheSize(100 * 1024 * 1024); // 100MB
}

int main() {
    std::cout << "CHTL Fragment Cache Test Suite\n" << std::endl;
    
    testCachePerformance();
    testCachePersistence();
    testCacheEviction();
    
    std::cout << "\nAll tests completed!" << std::endl;
    
    return 0;
}