#include "FragmentCache.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <functional>

namespace chtl {
namespace cache {

FragmentCache::FragmentCache(size_t maxCacheSize) 
    : maxCacheSize_(maxCacheSize)
    , currentCacheSize_(0)
    , enabled_(true) {
}

std::optional<std::vector<scanner::CodeFragment>> FragmentCache::get(const std::string& source) {
    if (!enabled_) {
        return std::nullopt;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string hash = calculateHash(source);
    auto it = cache_.find(hash);
    
    if (it != cache_.end()) {
        // 更新访问信息
        it->second.timestamp = std::chrono::steady_clock::now();
        it->second.hitCount++;
        stats_.totalHits++;
        
        // 更新LRU列表
        auto lruIt = std::find(lruList_.begin(), lruList_.end(), hash);
        if (lruIt != lruList_.end()) {
            lruList_.erase(lruIt);
        }
        lruList_.push_back(hash);
        
        return it->second.fragments;
    }
    
    stats_.totalMisses++;
    return std::nullopt;
}

void FragmentCache::put(const std::string& source, const std::vector<scanner::CodeFragment>& fragments) {
    if (!enabled_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string hash = calculateHash(source);
    size_t fragmentSize = estimateFragmentSize(fragments);
    
    // 确保有足够的空间
    ensureCapacity(fragmentSize);
    
    CacheEntry entry{
        fragments,
        std::chrono::steady_clock::now(),
        0,
        source.size(),
        hash
    };
    
    // 如果已存在，先移除旧的
    auto existing = cache_.find(hash);
    if (existing != cache_.end()) {
        currentCacheSize_ -= estimateFragmentSize(existing->second.fragments);
        auto lruIt = std::find(lruList_.begin(), lruList_.end(), hash);
        if (lruIt != lruList_.end()) {
            lruList_.erase(lruIt);
        }
    }
    
    cache_[hash] = std::move(entry);
    currentCacheSize_ += fragmentSize;
    lruList_.push_back(hash);
    
    // 更新统计
    stats_.currentSize = cache_.size();
    stats_.maxSize = std::max(stats_.maxSize, stats_.currentSize);
}

void FragmentCache::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
    lruList_.clear();
    currentCacheSize_ = 0;
    stats_.currentSize = 0;
}

CacheStats FragmentCache::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

void FragmentCache::setMaxCacheSize(size_t sizeInBytes) {
    std::lock_guard<std::mutex> lock(mutex_);
    maxCacheSize_ = sizeInBytes;
    ensureCapacity(0);
}

void FragmentCache::warmUp(const std::vector<std::string>& sources) {
    scanner::CHTLUnifiedScanner scanner;
    
    for (const auto& source : sources) {
        auto fragments = scanner.scan(source);
        put(source, fragments);
    }
}

bool FragmentCache::saveToDisk(const std::string& filepath) const {
    if (!config_.enablePersistence) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // 写入缓存条目数量
        size_t entryCount = cache_.size();
        file.write(reinterpret_cast<const char*>(&entryCount), sizeof(entryCount));
        
        // 写入每个缓存条目
        for (const auto& [hash, entry] : cache_) {
            // 写入hash长度和内容
            size_t hashLen = hash.length();
            file.write(reinterpret_cast<const char*>(&hashLen), sizeof(hashLen));
            file.write(hash.c_str(), hashLen);
            
            // 写入片段数量
            size_t fragmentCount = entry.fragments.size();
            file.write(reinterpret_cast<const char*>(&fragmentCount), sizeof(fragmentCount));
            
            // 写入每个片段
            for (const auto& fragment : entry.fragments) {
                // 写入片段类型
                file.write(reinterpret_cast<const char*>(&fragment.type), sizeof(fragment.type));
                
                // 写入内容
                size_t contentLen = fragment.content.length();
                file.write(reinterpret_cast<const char*>(&contentLen), sizeof(contentLen));
                file.write(fragment.content.c_str(), contentLen);
                
                // 写入位置信息
                file.write(reinterpret_cast<const char*>(&fragment.startLine), sizeof(fragment.startLine));
                file.write(reinterpret_cast<const char*>(&fragment.startColumn), sizeof(fragment.startColumn));
                file.write(reinterpret_cast<const char*>(&fragment.endLine), sizeof(fragment.endLine));
                file.write(reinterpret_cast<const char*>(&fragment.endColumn), sizeof(fragment.endColumn));
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool FragmentCache::loadFromDisk(const std::string& filepath) {
    if (!config_.enablePersistence) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        clear();
        
        // 读取缓存条目数量
        size_t entryCount;
        file.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount));
        
        // 读取每个缓存条目
        for (size_t i = 0; i < entryCount; ++i) {
            // 读取hash
            size_t hashLen;
            file.read(reinterpret_cast<char*>(&hashLen), sizeof(hashLen));
            std::string hash(hashLen, '\0');
            file.read(&hash[0], hashLen);
            
            // 读取片段数量
            size_t fragmentCount;
            file.read(reinterpret_cast<char*>(&fragmentCount), sizeof(fragmentCount));
            
            std::vector<scanner::CodeFragment> fragments;
            fragments.reserve(fragmentCount);
            
            // 读取每个片段
            for (size_t j = 0; j < fragmentCount; ++j) {
                scanner::CodeFragment fragment;
                
                // 读取片段类型
                file.read(reinterpret_cast<char*>(&fragment.type), sizeof(fragment.type));
                
                // 读取内容
                size_t contentLen;
                file.read(reinterpret_cast<char*>(&contentLen), sizeof(contentLen));
                fragment.content.resize(contentLen);
                file.read(&fragment.content[0], contentLen);
                
                // 读取位置信息
                file.read(reinterpret_cast<char*>(&fragment.startLine), sizeof(fragment.startLine));
                file.read(reinterpret_cast<char*>(&fragment.startColumn), sizeof(fragment.startColumn));
                file.read(reinterpret_cast<char*>(&fragment.endLine), sizeof(fragment.endLine));
                file.read(reinterpret_cast<char*>(&fragment.endColumn), sizeof(fragment.endColumn));
                
                fragments.push_back(std::move(fragment));
            }
            
            // 创建缓存条目
            CacheEntry entry{
                std::move(fragments),
                std::chrono::steady_clock::now(),
                0,
                0,
                hash
            };
            
            cache_[hash] = std::move(entry);
            lruList_.push_back(hash);
        }
        
        // 重新计算缓存大小
        currentCacheSize_ = 0;
        for (const auto& [hash, entry] : cache_) {
            currentCacheSize_ += estimateFragmentSize(entry.fragments);
        }
        
        stats_.currentSize = cache_.size();
        
        return true;
    } catch (...) {
        return false;
    }
}

void FragmentCache::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
}

bool FragmentCache::isEnabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_;
}

std::string FragmentCache::calculateHash(const std::string& source) const {
    // 使用std::hash作为简单的哈希实现
    // 在生产环境中应该使用更强的哈希算法如SHA256
    std::hash<std::string> hasher;
    size_t hashValue = hasher(source);
    
    std::stringstream ss;
    ss << std::hex << hashValue;
    return ss.str();
}

size_t FragmentCache::estimateFragmentSize(const std::vector<scanner::CodeFragment>& fragments) const {
    size_t size = 0;
    for (const auto& fragment : fragments) {
        size += sizeof(scanner::CodeFragment);
        size += fragment.content.capacity();
        // 估算其他成员的大小
        size += 64; // 预估其他字段占用
    }
    return size;
}

void FragmentCache::evictLRU() {
    if (lruList_.empty()) {
        return;
    }
    
    // 从LRU列表头部移除（最久未使用）
    std::string hashToEvict = lruList_.front();
    lruList_.erase(lruList_.begin());
    
    auto it = cache_.find(hashToEvict);
    if (it != cache_.end()) {
        currentCacheSize_ -= estimateFragmentSize(it->second.fragments);
        cache_.erase(it);
        stats_.totalEvictions++;
        stats_.currentSize--;
    }
}

void FragmentCache::ensureCapacity(size_t requiredSize) {
    while (currentCacheSize_ + requiredSize > maxCacheSize_ && !cache_.empty()) {
        evictLRU();
    }
}

// FragmentCacheManager 实现
FragmentCacheManager& FragmentCacheManager::getInstance() {
    static FragmentCacheManager instance;
    return instance;
}

FragmentCacheManager::FragmentCacheManager() 
    : cache_(std::make_unique<FragmentCache>()) {
}

FragmentCache& FragmentCacheManager::getCache() {
    return *cache_;
}

void FragmentCacheManager::configure(size_t maxSize, bool enablePersistence) {
    cache_->setMaxCacheSize(maxSize);
    // 可以扩展配置其他选项
}

} // namespace cache
} // namespace chtl