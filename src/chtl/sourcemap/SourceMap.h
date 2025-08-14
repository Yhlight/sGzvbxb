#ifndef CHTL_SOURCE_MAP_H
#define CHTL_SOURCE_MAP_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>

namespace chtl {
namespace sourcemap {

// 源码位置
struct SourcePosition {
    size_t line;
    size_t column;
    
    SourcePosition(size_t l = 0, size_t c = 0) : line(l), column(c) {}
};

// 映射项
struct Mapping {
    // 生成的代码位置
    SourcePosition generated;
    
    // 原始源码信息
    size_t sourceIndex;
    SourcePosition original;
    size_t nameIndex;  // 可选的名称索引
    
    Mapping() : sourceIndex(0), nameIndex(static_cast<size_t>(-1)) {}
};

// Source Map生成器
class SourceMapGenerator {
public:
    SourceMapGenerator();
    
    // 设置生成的文件名
    void setFile(const std::string& file);
    
    // 设置源码根目录
    void setSourceRoot(const std::string& root);
    
    // 添加源文件
    size_t addSource(const std::string& source, const std::string& content = "");
    
    // 添加名称
    size_t addName(const std::string& name);
    
    // 添加映射
    void addMapping(
        size_t generatedLine, size_t generatedColumn,
        size_t sourceIndex,
        size_t originalLine, size_t originalColumn,
        size_t nameIndex = static_cast<size_t>(-1)
    );
    
    // 快捷方法
    void addMapping(const Mapping& mapping);
    
    // 生成Source Map JSON
    std::string toJSON() const;
    
    // 生成内联Source Map
    std::string toInlineDataURL() const;
    
private:
    std::string file_;
    std::string sourceRoot_;
    std::vector<std::string> sources_;
    std::vector<std::string> sourcesContent_;
    std::vector<std::string> names_;
    std::vector<Mapping> mappings_;
    
    // VLQ编码
    std::string encodeVLQ(int value) const;
    std::string encodeMappings() const;
};

// Source Map消费者 - 用于调试
class SourceMapConsumer {
public:
    explicit SourceMapConsumer(const std::string& sourceMapJSON);
    
    // 查找原始位置
    struct OriginalPosition {
        std::string source;
        size_t line;
        size_t column;
        std::string name;
        bool found;
    };
    
    OriginalPosition originalPositionFor(size_t line, size_t column) const;
    
    // 查找生成的位置
    struct GeneratedPosition {
        size_t line;
        size_t column;
        bool found;
    };
    
    GeneratedPosition generatedPositionFor(
        const std::string& source,
        size_t line,
        size_t column
    ) const;
    
private:
    std::string file_;
    std::string sourceRoot_;
    std::vector<std::string> sources_;
    std::vector<std::string> names_;
    std::vector<Mapping> mappings_;
    
    void parseJSON(const std::string& json);
    void decodeMappings(const std::string& mappings);
    int decodeVLQ(const std::string& encoded, size_t& pos) const;
};

// Source Map上下文 - 用于编译过程中跟踪位置
class SourceMapContext {
public:
    SourceMapContext();
    
    // 进入新的源文件
    void enterSource(const std::string& filename);
    void exitSource();
    
    // 更新位置
    void setOriginalPosition(size_t line, size_t column);
    void setGeneratedPosition(size_t line, size_t column);
    
    // 记录映射
    void recordMapping(const std::string& name = "");
    
    // 获取生成器
    SourceMapGenerator& getGenerator() { return generator_; }
    const SourceMapGenerator& getGenerator() const { return generator_; }
    
private:
    SourceMapGenerator generator_;
    
    struct SourceContext {
        std::string filename;
        size_t sourceIndex;
        SourcePosition originalPos;
        SourcePosition generatedPos;
    };
    
    std::vector<SourceContext> sourceStack_;
    SourceContext* currentSource_;
};

// 辅助类 - 自动跟踪源码位置
class SourceLocationTracker {
public:
    SourceLocationTracker(
        SourceMapContext& context,
        size_t line,
        size_t column
    );
    
    ~SourceLocationTracker();
    
    void setName(const std::string& name);
    
private:
    SourceMapContext& context_;
    std::string name_;
    bool recorded_;
};

// 集成到编译器的Source Map构建器
class CompilerSourceMapBuilder {
public:
    CompilerSourceMapBuilder();
    
    // 开始编译文件
    void startFile(const std::string& sourceFile, const std::string& outputFile);
    
    // 处理代码片段
    void processFragment(
        const std::string& sourceContent,
        const std::string& generatedContent,
        size_t sourceLine,
        size_t sourceColumn
    );
    
    // 完成编译
    std::string finishFile();
    
    // 获取所有Source Map
    std::unordered_map<std::string, std::string> getAllSourceMaps() const;
    
private:
    struct FileContext {
        std::string sourceFile;
        std::string outputFile;
        std::unique_ptr<SourceMapGenerator> generator;
        size_t currentGeneratedLine;
        size_t currentGeneratedColumn;
    };
    
    std::unordered_map<std::string, FileContext> fileContexts_;
    FileContext* currentContext_;
    
    void updateGeneratedPosition(const std::string& content);
};

} // namespace sourcemap
} // namespace chtl

#endif // CHTL_SOURCE_MAP_H