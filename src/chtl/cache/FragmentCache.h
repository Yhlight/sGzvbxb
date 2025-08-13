#ifndef CHTL_FRAGMENT_CACHE_H
#define CHTL_FRAGMENT_CACHE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <optional>
#include "../scanner/CHTLUnifiedScanner.h"

namespace chtl {
namespace cache {

// 缓存键，包含文件路径和内容哈希
struct CacheKey {
    std::string filepath;
    std::size_t contentHash;
    std::chrono::system_clock::time_point timestamp;
    
    bool operator==(const CacheKey& other) const {
        return filepath == other.filepath && contentHash == other.contentHash;
    }
};

// 自定义哈希函数
struct CacheKeyHash {
    std::size_t operator()(const CacheKey& key) const {
        return std::hash<std::string>()(key.filepath) ^ 
               (std::hash<std::size_t>()(key.contentHash) << 1);
    }
};

// 缓存项
struct CacheEntry {
    std::vector<scanner::CodeFragment> fragments;
    std::chrono::system_clock::time_point lastAccess;
    size_t accessCount;
    size_t memorySize;  // 估算的内存占用
};

// 片段缓存管理器
class FragmentCache {
public:
    FragmentCache(size_t maxMemory = 100 * 1024 * 1024); // 默认100MB
    ~FragmentCache() = default;
    
    // 查找缓存
    std::optional<std::vector<scanner::CodeFragment>> get(
        const std::string& filepath, 
        const std::string& content
    );
    
    // 添加到缓存
    void put(
        const std::string& filepath,
        const std::string& content,
        const std::vector<scanner::CodeFragment>& fragments
    );
    
    // 清除缓存
    void clear();
    void evict(const std::string& filepath);
    
    // 统计信息
    struct Stats {
        size_t hitCount;
        size_t missCount;
        size_t totalMemory;
        size_t itemCount;
        double hitRate() const {
            size_t total = hitCount + missCount;
            return total > 0 ? static_cast<double>(hitCount) / total : 0.0;
        }
    };
    
    Stats getStats() const;
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<CacheKey, CacheEntry, CacheKeyHash> cache_;
    
    size_t maxMemory_;
    size_t currentMemory_;
    
    // 统计
    mutable size_t hitCount_;
    mutable size_t missCount_;
    
    // LRU驱逐策略
    void evictLRU();
    size_t estimateMemorySize(const std::vector<scanner::CodeFragment>& fragments);
    std::size_t computeHash(const std::string& content);
};

// 全局缓存实例
class FragmentCacheManager {
public:
    static FragmentCacheManager& getInstance() {
        static FragmentCacheManager instance;
        return instance;
    }
    
    FragmentCache& getCache() { return cache_; }
    
    // 启用/禁用缓存
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
private:
    FragmentCacheManager() : enabled_(true) {}
    FragmentCache cache_;
    bool enabled_;
};

} // namespace cache
} // namespace chtl

#endif // CHTL_FRAGMENT_CACHE_H