#ifndef CHTL_SOURCE_MAP_GENERATOR_H
#define CHTL_SOURCE_MAP_GENERATOR_H

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include "../scanner/CHTLUnifiedScanner.h"

namespace chtl {
namespace sourcemap {

// Source Map V3规范的映射结构
struct Mapping {
    // 生成文件的位置
    size_t generatedLine;
    size_t generatedColumn;
    
    // 源文件的位置
    size_t sourceLine;
    size_t sourceColumn;
    
    // 源文件索引
    size_t sourceIndex;
    
    // 名称索引（可选）
    int nameIndex = -1;
};

// Source Map生成器
class SourceMapGenerator {
public:
    SourceMapGenerator();
    ~SourceMapGenerator();
    
    // 设置生成的文件名
    void setFile(const std::string& file);
    
    // 设置源根目录
    void setSourceRoot(const std::string& root);
    
    // 添加源文件
    size_t addSource(const std::string& source);
    
    // 添加名称
    size_t addName(const std::string& name);
    
    // 添加映射
    void addMapping(const Mapping& mapping);
    
    // 添加映射（简化版本）
    void addMapping(
        size_t generatedLine, size_t generatedColumn,
        size_t sourceLine, size_t sourceColumn,
        size_t sourceIndex
    );
    
    // 生成Source Map JSON
    std::string generateJSON() const;
    
    // 生成内联Source Map（base64编码）
    std::string generateInline() const;
    
    // 获取Source Map注释
    std::string getComment(bool inlineMap = false) const;
    
private:
    // VLQ编码
    std::string encodeVLQ(int value) const;
    
    // 生成mappings字符串
    std::string generateMappings() const;
    
    // Base64编码
    std::string base64Encode(const std::string& input) const;
    
private:
    std::string file_;
    std::string sourceRoot_;
    std::vector<std::string> sources_;
    std::vector<std::string> names_;
    std::vector<Mapping> mappings_;
};

// 片段映射跟踪器
class FragmentMappingTracker {
public:
    FragmentMappingTracker();
    
    // 开始跟踪新片段
    void startFragment(const scanner::CodeFragment& fragment);
    
    // 记录生成代码的位置
    void recordGenerated(size_t line, size_t column);
    
    // 记录源代码映射
    void recordMapping(
        size_t generatedLine, size_t generatedColumn,
        size_t sourceLine, size_t sourceColumn
    );
    
    // 结束当前片段
    void endFragment();
    
    // 获取所有映射
    std::vector<Mapping> getMappings() const;
    
private:
    struct FragmentContext {
        scanner::CodeFragment fragment;
        size_t sourceIndex;
        std::vector<Mapping> mappings;
    };
    
    std::vector<FragmentContext> contexts_;
    FragmentContext* currentContext_;
    size_t currentGeneratedLine_;
    size_t currentGeneratedColumn_;
};

// Source Map管理器
class SourceMapManager {
public:
    static SourceMapManager& getInstance();
    
    // 创建新的Source Map生成器
    std::shared_ptr<SourceMapGenerator> createGenerator(const std::string& outputFile);
    
    // 创建片段映射跟踪器
    std::shared_ptr<FragmentMappingTracker> createTracker();
    
    // 启用/禁用Source Map生成
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    // 设置Source Map选项
    struct Options {
        bool inlineMap = false;       // 内联Source Map
        bool includeContent = false;  // 包含源内容
        std::string sourceRoot;       // 源根目录
    };
    
    void setOptions(const Options& options);
    const Options& getOptions() const;
    
private:
    SourceMapManager();
    bool enabled_;
    Options options_;
};

// 代码生成器辅助类
class SourceMapBuilder {
public:
    SourceMapBuilder(std::shared_ptr<SourceMapGenerator> generator);
    
    // 写入字符串并跟踪位置
    void write(const std::string& text);
    
    // 写入带映射的字符串
    void writeWithMapping(
        const std::string& text,
        size_t sourceLine,
        size_t sourceColumn,
        size_t sourceIndex
    );
    
    // 写入新行
    void writeLine(const std::string& text = "");
    
    // 获取当前位置
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    
    // 获取生成的代码
    std::string getCode() const { return output_.str(); }
    
    // 获取Source Map
    std::string getSourceMap() const;
    
private:
    std::shared_ptr<SourceMapGenerator> generator_;
    std::stringstream output_;
    size_t currentLine_;
    size_t currentColumn_;
};

} // namespace sourcemap
} // namespace chtl

#endif // CHTL_SOURCE_MAP_GENERATOR_H