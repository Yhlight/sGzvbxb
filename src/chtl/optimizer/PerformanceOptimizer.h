#pragma once

#include <memory>
#include <unordered_map>
#include <chrono>
#include <string>

namespace chtl {
namespace optimizer {

// 性能统计信息
struct PerformanceStats {
    std::chrono::microseconds parseTime;
    std::chrono::microseconds codeGenTime;
    std::chrono::microseconds totalTime;
    size_t memoryUsage;
    size_t cacheHits;
    size_t cacheMisses;
};

// 片段缓存
class FragmentCache {
public:
    FragmentCache(size_t maxSize = 1000);
    
    // 检查缓存中是否存在
    bool contains(const std::string& key) const;
    
    // 获取缓存内容
    std::string get(const std::string& key);
    
    // 添加到缓存
    void put(const std::string& key, const std::string& value);
    
    // 清空缓存
    void clear();
    
    // 获取缓存统计
    size_t getHits() const { return hits_; }
    size_t getMisses() const { return misses_; }
    
private:
    struct CacheEntry {
        std::string value;
        std::chrono::steady_clock::time_point lastAccess;
        size_t accessCount;
    };
    
    std::unordered_map<std::string, CacheEntry> cache_;
    size_t maxSize_;
    size_t hits_ = 0;
    size_t misses_ = 0;
    
    // LRU 驱逐策略
    void evictLRU();
};

// 内存池管理
template<typename T>
class MemoryPool {
public:
    MemoryPool(size_t blockSize = 1024);
    ~MemoryPool();
    
    // 分配内存
    T* allocate();
    
    // 释放内存
    void deallocate(T* ptr);
    
    // 重置内存池
    void reset();
    
private:
    struct Block {
        T* memory;
        size_t used;
        Block* next;
    };
    
    Block* currentBlock_;
    Block* firstBlock_;
    size_t blockSize_;
    std::vector<T*> freeList_;
};

// 性能优化器主类
class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    
    // 启用/禁用优化功能
    void enableFragmentCache(bool enable) { fragmentCacheEnabled_ = enable; }
    void enableMemoryPool(bool enable) { memoryPoolEnabled_ = enable; }
    void enableParallelParsing(bool enable) { parallelParsingEnabled_ = enable; }
    
    // 获取片段缓存
    FragmentCache& getFragmentCache() { return fragmentCache_; }
    
    // 记录性能数据
    void recordParseTime(std::chrono::microseconds time);
    void recordCodeGenTime(std::chrono::microseconds time);
    void recordMemoryUsage(size_t bytes);
    
    // 获取性能统计
    PerformanceStats getStats() const;
    
    // 重置统计数据
    void resetStats();
    
private:
    FragmentCache fragmentCache_;
    PerformanceStats stats_;
    
    bool fragmentCacheEnabled_ = true;
    bool memoryPoolEnabled_ = true;
    bool parallelParsingEnabled_ = false;
};

} // namespace optimizer
} // namespace chtl