#include "FragmentCache.h"
#include <algorithm>
#include <functional>

namespace chtl {
namespace cache {

FragmentCache::FragmentCache(size_t maxMemory) 
    : maxMemory_(maxMemory), currentMemory_(0), hitCount_(0), missCount_(0) {
}

std::optional<std::vector<scanner::CodeFragment>> FragmentCache::get(
    const std::string& filepath, 
    const std::string& content) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    CacheKey key{filepath, computeHash(content), std::chrono::system_clock::now()};
    
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        // 更新访问信息
        it->second.lastAccess = std::chrono::system_clock::now();
        it->second.accessCount++;
        hitCount_++;
        
        return it->second.fragments;
    }
    
    missCount_++;
    return std::nullopt;
}

void FragmentCache::put(
    const std::string& filepath,
    const std::string& content,
    const std::vector<scanner::CodeFragment>& fragments) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t memSize = estimateMemorySize(fragments);
    
    // 检查是否需要驱逐
    while (currentMemory_ + memSize > maxMemory_ && !cache_.empty()) {
        evictLRU();
    }
    
    CacheKey key{filepath, computeHash(content), std::chrono::system_clock::now()};
    CacheEntry entry{
        fragments,
        std::chrono::system_clock::now(),
        0,
        memSize
    };
    
    cache_[key] = std::move(entry);
    currentMemory_ += memSize;
}

void FragmentCache::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
    currentMemory_ = 0;
    hitCount_ = 0;
    missCount_ = 0;
}

void FragmentCache::evict(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = cache_.begin();
    while (it != cache_.end()) {
        if (it->first.filepath == filepath) {
            currentMemory_ -= it->second.memorySize;
            it = cache_.erase(it);
        } else {
            ++it;
        }
    }
}

FragmentCache::Stats FragmentCache::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return Stats{
        hitCount_,
        missCount_,
        currentMemory_,
        cache_.size()
    };
}

void FragmentCache::evictLRU() {
    if (cache_.empty()) return;
    
    // 找到最久未访问的项
    auto oldest = cache_.begin();
    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
        if (it->second.lastAccess < oldest->second.lastAccess) {
            oldest = it;
        }
    }
    
    currentMemory_ -= oldest->second.memorySize;
    cache_.erase(oldest);
}

size_t FragmentCache::estimateMemorySize(const std::vector<scanner::CodeFragment>& fragments) {
    size_t size = 0;
    for (const auto& fragment : fragments) {
        size += sizeof(scanner::CodeFragment);
        size += fragment.content.capacity();
        // CodeFragment没有filename成员
    }
    return size;
}

std::size_t FragmentCache::computeHash(const std::string& content) {
    return std::hash<std::string>()(content);
}

} // namespace cache
} // namespace chtl