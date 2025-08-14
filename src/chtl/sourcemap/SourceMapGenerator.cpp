#include "SourceMapGenerator.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace chtl {
namespace sourcemap {

// Base64字符表
static const char base64_chars[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

// VLQ编码字符表
static const char vlq_chars[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

SourceMapGenerator::SourceMapGenerator() {
}

SourceMapGenerator::~SourceMapGenerator() = default;

void SourceMapGenerator::setFile(const std::string& file) {
    file_ = file;
}

void SourceMapGenerator::setSourceRoot(const std::string& root) {
    sourceRoot_ = root;
}

size_t SourceMapGenerator::addSource(const std::string& source) {
    auto it = std::find(sources_.begin(), sources_.end(), source);
    if (it != sources_.end()) {
        return std::distance(sources_.begin(), it);
    }
    sources_.push_back(source);
    return sources_.size() - 1;
}

size_t SourceMapGenerator::addName(const std::string& name) {
    auto it = std::find(names_.begin(), names_.end(), name);
    if (it != names_.end()) {
        return std::distance(names_.begin(), it);
    }
    names_.push_back(name);
    return names_.size() - 1;
}

void SourceMapGenerator::addMapping(const Mapping& mapping) {
    mappings_.push_back(mapping);
}

void SourceMapGenerator::addMapping(
    size_t generatedLine, size_t generatedColumn,
    size_t sourceLine, size_t sourceColumn,
    size_t sourceIndex) {
    
    Mapping mapping;
    mapping.generatedLine = generatedLine;
    mapping.generatedColumn = generatedColumn;
    mapping.sourceLine = sourceLine;
    mapping.sourceColumn = sourceColumn;
    mapping.sourceIndex = sourceIndex;
    mappings_.push_back(mapping);
}

std::string SourceMapGenerator::generateJSON() const {
    std::stringstream json;
    json << "{\n";
    json << "  \"version\": 3,\n";
    
    if (!file_.empty()) {
        json << "  \"file\": \"" << file_ << "\",\n";
    }
    
    if (!sourceRoot_.empty()) {
        json << "  \"sourceRoot\": \"" << sourceRoot_ << "\",\n";
    }
    
    // Sources数组
    json << "  \"sources\": [";
    for (size_t i = 0; i < sources_.size(); ++i) {
        if (i > 0) json << ", ";
        json << "\"" << sources_[i] << "\"";
    }
    json << "],\n";
    
    // Names数组
    json << "  \"names\": [";
    for (size_t i = 0; i < names_.size(); ++i) {
        if (i > 0) json << ", ";
        json << "\"" << names_[i] << "\"";
    }
    json << "],\n";
    
    // Mappings
    json << "  \"mappings\": \"" << generateMappings() << "\"\n";
    json << "}";
    
    return json.str();
}

std::string SourceMapGenerator::generateInline() const {
    std::string json = generateJSON();
    return "data:application/json;base64," + base64Encode(json);
}

std::string SourceMapGenerator::getComment(bool inlineMap) const {
    if (inlineMap) {
        return "//# sourceMappingURL=" + generateInline();
    } else {
        return "//# sourceMappingURL=" + file_ + ".map";
    }
}

std::string SourceMapGenerator::encodeVLQ(int value) const {
    std::string result;
    
    // 转换为VLQ格式
    int vlq = value < 0 ? ((-value) << 1) | 1 : value << 1;
    
    do {
        int digit = vlq & 0x1F;
        vlq >>= 5;
        
        if (vlq > 0) {
            digit |= 0x20; // 继续位
        }
        
        result += vlq_chars[digit];
    } while (vlq > 0);
    
    return result;
}

std::string SourceMapGenerator::generateMappings() const {
    if (mappings_.empty()) {
        return "";
    }
    
    // 按生成位置排序映射
    std::vector<Mapping> sortedMappings = mappings_;
    std::sort(sortedMappings.begin(), sortedMappings.end(),
        [](const Mapping& a, const Mapping& b) {
            if (a.generatedLine != b.generatedLine) {
                return a.generatedLine < b.generatedLine;
            }
            return a.generatedColumn < b.generatedColumn;
        });
    
    std::stringstream result;
    
    size_t previousGeneratedLine = 0;
    size_t previousGeneratedColumn = 0;
    size_t previousSourceIndex = 0;
    size_t previousSourceLine = 0;
    size_t previousSourceColumn = 0;
    int previousNameIndex = 0;
    
    for (const auto& mapping : sortedMappings) {
        // 处理新行
        while (previousGeneratedLine < mapping.generatedLine) {
            result << ";";
            previousGeneratedLine++;
            previousGeneratedColumn = 0;
        }
        
        // 添加段分隔符
        if (previousGeneratedColumn > 0) {
            result << ",";
        }
        
        // 编码生成列（相对值）
        result << encodeVLQ(static_cast<int>(mapping.generatedColumn) - 
                           static_cast<int>(previousGeneratedColumn));
        previousGeneratedColumn = mapping.generatedColumn;
        
        // 编码源文件索引（相对值）
        result << encodeVLQ(static_cast<int>(mapping.sourceIndex) - 
                           static_cast<int>(previousSourceIndex));
        previousSourceIndex = mapping.sourceIndex;
        
        // 编码源行（相对值）
        result << encodeVLQ(static_cast<int>(mapping.sourceLine) - 
                           static_cast<int>(previousSourceLine));
        previousSourceLine = mapping.sourceLine;
        
        // 编码源列（相对值）
        result << encodeVLQ(static_cast<int>(mapping.sourceColumn) - 
                           static_cast<int>(previousSourceColumn));
        previousSourceColumn = mapping.sourceColumn;
        
        // 编码名称索引（如果有）
        if (mapping.nameIndex >= 0) {
            result << encodeVLQ(mapping.nameIndex - previousNameIndex);
            previousNameIndex = mapping.nameIndex;
        }
    }
    
    return result.str();
}

std::string SourceMapGenerator::base64Encode(const std::string& input) const {
    std::string result;
    int val = 0;
    int valb = -6;
    
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6) {
        result.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    
    while (result.size() % 4) {
        result.push_back('=');
    }
    
    return result;
}

// FragmentMappingTracker 实现
FragmentMappingTracker::FragmentMappingTracker()
    : currentContext_(nullptr)
    , currentGeneratedLine_(0)
    , currentGeneratedColumn_(0) {
}

void FragmentMappingTracker::startFragment(const scanner::CodeFragment& fragment) {
    FragmentContext context;
    context.fragment = fragment;
    context.sourceIndex = contexts_.size();
    contexts_.push_back(context);
    currentContext_ = &contexts_.back();
}

void FragmentMappingTracker::recordGenerated(size_t line, size_t column) {
    currentGeneratedLine_ = line;
    currentGeneratedColumn_ = column;
}

void FragmentMappingTracker::recordMapping(
    size_t generatedLine, size_t generatedColumn,
    size_t sourceLine, size_t sourceColumn) {
    
    if (!currentContext_) {
        return;
    }
    
    Mapping mapping;
    mapping.generatedLine = generatedLine;
    mapping.generatedColumn = generatedColumn;
    mapping.sourceLine = sourceLine + currentContext_->fragment.startLine - 1;
    mapping.sourceColumn = sourceColumn + currentContext_->fragment.startColumn - 1;
    mapping.sourceIndex = currentContext_->sourceIndex;
    
    currentContext_->mappings.push_back(mapping);
}

void FragmentMappingTracker::endFragment() {
    currentContext_ = nullptr;
}

std::vector<Mapping> FragmentMappingTracker::getMappings() const {
    std::vector<Mapping> allMappings;
    
    for (const auto& context : contexts_) {
        allMappings.insert(allMappings.end(),
                          context.mappings.begin(),
                          context.mappings.end());
    }
    
    return allMappings;
}

// SourceMapManager 实现
SourceMapManager& SourceMapManager::getInstance() {
    static SourceMapManager instance;
    return instance;
}

SourceMapManager::SourceMapManager() : enabled_(true) {
}

std::shared_ptr<SourceMapGenerator> SourceMapManager::createGenerator(
    const std::string& outputFile) {
    
    if (!enabled_) {
        return nullptr;
    }
    
    auto generator = std::make_shared<SourceMapGenerator>();
    generator->setFile(outputFile);
    
    if (!options_.sourceRoot.empty()) {
        generator->setSourceRoot(options_.sourceRoot);
    }
    
    return generator;
}

std::shared_ptr<FragmentMappingTracker> SourceMapManager::createTracker() {
    if (!enabled_) {
        return nullptr;
    }
    
    return std::make_shared<FragmentMappingTracker>();
}

void SourceMapManager::setEnabled(bool enabled) {
    enabled_ = enabled;
}

bool SourceMapManager::isEnabled() const {
    return enabled_;
}

void SourceMapManager::setOptions(const Options& options) {
    options_ = options;
}

const SourceMapManager::Options& SourceMapManager::getOptions() const {
    return options_;
}

// SourceMapBuilder 实现
SourceMapBuilder::SourceMapBuilder(std::shared_ptr<SourceMapGenerator> generator)
    : generator_(generator)
    , currentLine_(0)
    , currentColumn_(0) {
}

void SourceMapBuilder::write(const std::string& text) {
    for (char ch : text) {
        output_ << ch;
        if (ch == '\n') {
            currentLine_++;
            currentColumn_ = 0;
        } else {
            currentColumn_++;
        }
    }
}

void SourceMapBuilder::writeWithMapping(
    const std::string& text,
    size_t sourceLine,
    size_t sourceColumn,
    size_t sourceIndex) {
    
    if (generator_) {
        generator_->addMapping(
            currentLine_, currentColumn_,
            sourceLine, sourceColumn,
            sourceIndex
        );
    }
    
    write(text);
}

void SourceMapBuilder::writeLine(const std::string& text) {
    write(text);
    if (!text.empty() && text.back() != '\n') {
        write("\n");
    }
}

std::string SourceMapBuilder::getSourceMap() const {
    if (!generator_) {
        return "";
    }
    
    return generator_->generateJSON();
}

} // namespace sourcemap
} // namespace chtl