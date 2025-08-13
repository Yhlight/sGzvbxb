#ifndef CHTL_MAIN_COMPILER_H
#define CHTL_MAIN_COMPILER_H

#include <string>
#include <memory>
#include <vector>
#include "CHTLContext.h"
#include "scanner/CHTLUnifiedScanner.h"
#include "compiler/CompilerDispatcher.h"

namespace chtl {

/**
 * CHTL主编译器
 * 
 * 架构说明：
 * 1. 扫描器(Scanner)：负责精准切割代码为不同语言片段
 * 2. 编译器调度器(Dispatcher)：根据片段类型分发到对应编译器
 * 3. 四个独立编译器：
 *    - CHTL编译器（手写）
 *    - CHTL JS编译器（手写）
 *    - CSS编译器（ANTLR生成）
 *    - JavaScript编译器（ANTLR生成）
 * 
 * 编译流程：
 * 源代码 -> 扫描器切割 -> 分类片段 -> 各自编译器处理 -> 合并结果
 */
class CHTLMainCompiler {
public:
    CHTLMainCompiler();
    ~CHTLMainCompiler() = default;
    
    /**
     * 编译CHTL源文件
     * @param sourceFile 源文件路径
     * @param outputFile 输出文件路径
     * @return 是否成功
     */
    bool compileFile(const std::string& sourceFile, const std::string& outputFile);
    
    /**
     * 编译CHTL源代码字符串
     * @param source 源代码
     * @return 编译后的HTML
     */
    std::string compileString(const std::string& source);
    
    /**
     * 获取编译错误
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
    /**
     * 获取编译警告
     */
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    
    /**
     * 设置编译选项
     */
    void setMinifyCSS(bool minify);
    void setMinifyJS(bool minify);
    void setStrictMode(bool strict);
    
private:
    // 核心组件
    std::shared_ptr<CHTLContext> context_;
    std::unique_ptr<scanner::CHTLUnifiedScanner> scanner_;
    std::unique_ptr<compiler::CompilerDispatcher> dispatcher_;
    
    // 编译状态
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 初始化组件
    void initialize();
    
    // 执行编译流程
    compiler::CompileResult doCompile(const std::string& source);
};

} // namespace chtl

#endif // CHTL_MAIN_COMPILER_H