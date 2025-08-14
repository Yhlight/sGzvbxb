#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>

namespace chtl {

// 前向声明
namespace parser {
    class ParseContext;
}

/**
 * 自动化类/ID处理器
 * 负责处理局部样式块中的类选择器和ID选择器，自动为元素添加对应的类名或ID
 */
class AutoClassIdProcessor {
public:
    struct StyleBlockInfo {
        std::unordered_set<std::string> autoClasses;    // 自动检测到的类名
        std::unordered_set<std::string> autoIds;        // 自动检测到的ID
        std::string globalCss;                           // 需要提升到全局的CSS
        std::string inlineCss;                          // 内联样式
    };
    
    /**
     * 处理局部样式块
     * @param styleBlock 样式块的解析树节点
     * @return 处理结果，包含检测到的类名/ID和生成的CSS
     */
    static StyleBlockInfo processLocalStyleBlock(std::shared_ptr<parser::ParseContext> styleBlock);
    
    /**
     * 从选择器中提取类名
     * @param selector 选择器字符串（如 ".box"）
     * @return 类名（如 "box"），如果不是类选择器返回空字符串
     */
    static std::string extractClassName(const std::string& selector);
    
    /**
     * 从选择器中提取ID
     * @param selector 选择器字符串（如 "#header"）
     * @return ID名（如 "header"），如果不是ID选择器返回空字符串
     */
    static std::string extractIdName(const std::string& selector);
    
    /**
     * 处理上下文推断符号 &
     * @param selector 包含 & 的选择器
     * @param currentClass 当前元素的类名（可能为空）
     * @param currentId 当前元素的ID（可能为空）
     * @return 处理后的选择器
     */
    static std::string processContextSymbol(const std::string& selector, 
                                           const std::string& currentClass, 
                                           const std::string& currentId);
    
    /**
     * 合并类名列表
     * @param existingClasses 现有的类名（空格分隔）
     * @param newClasses 要添加的类名集合
     * @return 合并后的类名字符串
     */
    static std::string mergeClasses(const std::string& existingClasses, 
                                   const std::unordered_set<std::string>& newClasses);
    
private:
    /**
     * 生成CSS规则
     * @param selector 选择器
     * @param properties CSS属性
     * @return 格式化的CSS规则
     */
    static std::string generateCssRule(const std::string& selector, 
                                      const std::vector<std::pair<std::string, std::string>>& properties);
};

} // namespace chtl