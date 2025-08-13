#ifndef CHTL_PRECISION_SCANNER_H
#define CHTL_PRECISION_SCANNER_H

#include "CHTLUnifiedScanner.h"
#include <stack>

namespace chtl {
namespace scanner {

// 精确扫描器 - 对CHTL和CHTL JS进行字符级别的精确截断
// 对CSS和JS进行块级别的宽松处理
class PrecisionScanner : public CHTLUnifiedScanner {
public:
    PrecisionScanner();
    
    // 重写扫描方法，实现精确截断
    std::vector<CodeFragment> scan(const std::string& input) override;
    
private:
    enum class ScanMode {
        CHTL,        // CHTL模式 - 严格截断
        STYLE_BLOCK, // style块内 - 判断是局部还是全局
        SCRIPT_BLOCK,// script块内 - 混合处理
        CSS,         // CSS模式 - 宽松块处理
        JS,          // JS模式 - 宽松块处理  
        CHTL_JS      // CHTL JS模式 - 严格截断
    };
    
    struct ScanContext {
        ScanMode mode;
        size_t braceLevel;
        bool isGlobalStyle;  // 是否是全局style
        bool inElement;      // 是否在元素内部
    };
    
    std::stack<ScanContext> contextStack_;
    
    // 精确扫描方法
    void scanPrecise(std::vector<CodeFragment>& fragments);
    
    // CHTL特征的字符级别扫描
    bool scanCHTLFeature(std::vector<CodeFragment>& fragments);
    bool scanAtDirective(std::vector<CodeFragment>& fragments);
    bool scanTemplateDirective(std::vector<CodeFragment>& fragments);
    bool scanCustomDirective(std::vector<CodeFragment>& fragments);
    bool scanTextBlock(std::vector<CodeFragment>& fragments);
    
    // CHTL JS特征的字符级别扫描
    bool scanCHTLJSFeature(std::vector<CodeFragment>& fragments);
    bool scanInterpolation(std::vector<CodeFragment>& fragments);
    bool scanChainOperator(std::vector<CodeFragment>& fragments);
    bool scanListenMethod(std::vector<CodeFragment>& fragments);
    bool scanAnimateFunction(std::vector<CodeFragment>& fragments);
    
    // 块级别扫描（宽松）
    void scanCSSBlock(std::vector<CodeFragment>& fragments);
    void scanJSBlock(std::vector<CodeFragment>& fragments);
    
    // 辅助方法
    bool matchExact(const std::string& pattern);
    bool isAtBoundary();  // 检查是否在单词边界
    void skipWhitespace();
    std::string scanIdentifier();
    std::string scanUntil(char delimiter);
    std::string scanUntilPattern(const std::string& pattern);
    std::string scanBalancedBraces();
    
    // 上下文管理
    void pushContext(ScanMode mode);
    void popContext();
    ScanMode currentMode() const;
    bool isInScript() const;
    bool isInStyle() const;
};

} // namespace scanner
} // namespace chtl

#endif // CHTL_PRECISION_SCANNER_H