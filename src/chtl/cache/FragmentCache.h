#ifndef CHTL_FRAGMENT_CACHE_H
#define CHTL_FRAGMENT_CACHE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <vector>
#include <optional>
#include "../scanner/CHTLUnifiedScanner.h"

namespace chtl {
namespace cache {

// 缓存条目
struct CacheEntry {
    std::vector<scanner::CodeFragment> fragments;
    std::chrono::steady_clock::time_point timestamp;
    size_t hitCount;
    size_t fileSize;
    std::string hash;
};

// 缓存统计信息
struct CacheStats {
    size_t totalHits = 0;
    size_t totalMisses = 0;
    size_t totalEvictions = 0;
    size_t currentSize = 0;
    size_t maxSize = 0;
    double hitRate() const {
        size_t total = totalHits + totalMisses;
        return total > 0 ? static_cast<double>(totalHits) / total : 0.0;
    }
};

// 片段缓存管理器
class FragmentCache {
public:
    // 构造函数，可指定最大缓存大小（字节）
    explicit FragmentCache(size_t maxCacheSize = 100 * 1024 * 1024); // 默认100MB
    
    // 获取缓存的片段，如果不存在返回空
    std::optional<std::vector<scanner::CodeFragment>> get(const std::string& source);
    
    // 存储片段到缓存
    void put(const std::string& source, const std::vector<scanner::CodeFragment>& fragments);
    
    // 清空缓存
    void clear();
    
    // 获取缓存统计信息
    CacheStats getStats() const;
    
    // 设置最大缓存大小
    void setMaxCacheSize(size_t sizeInBytes);
    
    // 预热缓存（加载常用片段）
    void warmUp(const std::vector<std::string>& sources);
    
    // 持久化缓存到磁盘
    bool saveToDisk(const std::string& filepath) const;
    
    // 从磁盘加载缓存
    bool loadFromDisk(const std::string& filepath);
    
    // 启用/禁用缓存
    void setEnabled(bool enabled);
    bool isEnabled() const;

private:
    // 计算源代码的哈希值
    std::string calculateHash(const std::string& source) const;
    
    // 估算片段占用的内存大小
    size_t estimateFragmentSize(const std::vector<scanner::CodeFragment>& fragments) const;
    
    // LRU淘汰策略
    void evictLRU();
    
    // 检查并执行必要的淘汰
    void ensureCapacity(size_t requiredSize);

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, CacheEntry> cache_;
    size_t maxCacheSize_;
    size_t currentCacheSize_;
    CacheStats stats_;
    bool enabled_;
    
    // LRU tracking
    std::vector<std::string> lruList_;
    
    // 配置选项
    struct Config {
        bool enableCompression = false;
        bool enablePersistence = true;
        std::chrono::seconds ttl{3600}; // 1小时TTL
    } config_;
};

// 全局缓存实例管理器
class FragmentCacheManager {
public:
    static FragmentCacheManager& getInstance();
    
    FragmentCache& getCache();
    
    // 配置全局缓存
    void configure(size_t maxSize, bool enablePersistence = true);
    
private:
    FragmentCacheManager();
    std::unique_ptr<FragmentCache> cache_;
};

} // namespace cache
} // namespace chtl

#endif // CHTL_FRAGMENT_CACHE_H