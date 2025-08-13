#ifndef CHTL_TECHNICAL_DEBT_MANAGER_H
#define CHTL_TECHNICAL_DEBT_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace chtl {
namespace utils {

// 技术债务类型
enum class DebtType {
    TODO,           // 待实现功能
    FIXME,          // 需要修复的问题
    HACK,           // 临时解决方案
    REFACTOR,       // 需要重构
    PERFORMANCE,    // 性能问题
    SECURITY,       // 安全问题
    DEPRECATED      // 废弃代码
};

// 优先级
enum class DebtPriority {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4
};

// 技术债务项
struct TechnicalDebtItem {
    std::string id;
    DebtType type;
    DebtPriority priority;
    std::string file;
    size_t line;
    std::string description;
    std::string category;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::string assignee;
    std::vector<std::string> tags;
    
    // 估算的修复时间（小时）
    double estimatedHours;
    
    // 技术债务的影响范围
    struct Impact {
        bool affectsPerformance;
        bool affectsReliability;
        bool affectsSecurity;
        bool affectsMaintainability;
        std::string description;
    } impact;
};

// 技术债务管理器
class TechnicalDebtManager {
public:
    TechnicalDebtManager();
    
    // 扫描代码库中的技术债务
    void scanCodebase(const std::string& rootPath);
    
    // 添加技术债务项
    void addDebtItem(const TechnicalDebtItem& item);
    
    // 获取所有技术债务
    std::vector<TechnicalDebtItem> getAllDebts() const;
    
    // 按优先级获取
    std::vector<TechnicalDebtItem> getDebtsByPriority(DebtPriority minPriority) const;
    
    // 按类型获取
    std::vector<TechnicalDebtItem> getDebtsByType(DebtType type) const;
    
    // 生成报告
    std::string generateReport() const;
    std::string generateHTMLReport() const;
    std::string generateMarkdownReport() const;
    
    // 估算总体技术债务
    struct DebtMetrics {
        size_t totalItems;
        std::unordered_map<DebtType, size_t> byType;
        std::unordered_map<DebtPriority, size_t> byPriority;
        double totalEstimatedHours;
        std::vector<std::string> topFiles;  // 技术债务最多的文件
    };
    
    DebtMetrics calculateMetrics() const;
    
    // 导出/导入
    void exportToJSON(const std::string& filename) const;
    void importFromJSON(const std::string& filename);
    
private:
    std::vector<TechnicalDebtItem> debtItems_;
    std::unordered_map<std::string, std::vector<size_t>> debtByFile_;
    
    // 解析代码文件
    void parseFile(const std::string& filepath);
    
    // 分析注释中的技术债务
    TechnicalDebtItem parseDebtComment(
        const std::string& comment,
        const std::string& file,
        size_t line
    );
    
    // 计算优先级
    DebtPriority calculatePriority(const TechnicalDebtItem& item) const;
};

// 技术债务修复建议生成器
class DebtFixSuggestionGenerator {
public:
    DebtFixSuggestionGenerator();
    
    // 为特定技术债务生成修复建议
    struct FixSuggestion {
        std::string approach;
        std::vector<std::string> steps;
        double estimatedHours;
        std::vector<std::string> risks;
        std::vector<std::string> dependencies;
    };
    
    FixSuggestion generateSuggestion(const TechnicalDebtItem& debt);
    
private:
    // 不同类型债务的修复策略
    FixSuggestion suggestTodoFix(const TechnicalDebtItem& debt);
    FixSuggestion suggestPerformanceFix(const TechnicalDebtItem& debt);
    FixSuggestion suggestRefactorFix(const TechnicalDebtItem& debt);
};

// 代码清理助手
class CodeCleanupHelper {
public:
    CodeCleanupHelper();
    
    // 清理未使用的代码
    struct UnusedCode {
        std::string file;
        size_t line;
        std::string type;  // function, variable, class, include
        std::string name;
    };
    
    std::vector<UnusedCode> findUnusedCode(const std::string& rootPath);
    
    // 清理重复代码
    struct DuplicateCode {
        std::string file1;
        size_t line1;
        std::string file2;
        size_t line2;
        size_t similarity;  // 相似度百分比
        std::string snippet;
    };
    
    std::vector<DuplicateCode> findDuplicateCode(const std::string& rootPath);
    
    // 代码规范检查
    struct StyleViolation {
        std::string file;
        size_t line;
        std::string rule;
        std::string description;
        std::string suggestion;
    };
    
    std::vector<StyleViolation> checkCodeStyle(const std::string& rootPath);
    
    // 自动修复
    bool autoFix(const std::string& file, const std::vector<StyleViolation>& violations);
    
private:
    // 代码指纹计算（用于查找重复）
    std::string calculateFingerprint(const std::string& code);
    
    // 符号使用分析
    void analyzeSymbolUsage(const std::string& file);
};

} // namespace utils
} // namespace chtl

#endif // CHTL_TECHNICAL_DEBT_MANAGER_H