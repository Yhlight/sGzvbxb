# CHTL 统一架构实现总结

## 核心实现：基于上下文的智能切割

### 扫描器的关键设计

1. **宽严判机制**
   - **严判（CHTL/CHTL JS）**：以最小单元为单位，精确切割
   - **宽判（CSS/JS）**：可以包含较大代码块，但排除 CHTL/CHTL JS 部分

2. **上下文感知**
   - 同语言延续：`{{.box}}.listen()` 作为整体 CHTL JS 片段
   - 异语言截断：`{{.box}}.addEventListener()` 仅选择器是 CHTL JS

3. **智能边界检测**
   - CHTL JS 专有方法：`listen`, `delegate`, `animate`
   - 链式操作符：`->`
   - DOM 选择器：`{{selector}}`

### 编译器职责分配

| 代码类型 | 编译器 | 处理方式 |
|---------|--------|---------|
| 局部样式块 | CHTL | 整块传递，包括自动化类名等特性 |
| 全局样式块 | CSS + CHTL | CSS 为主，CHTL 语法切割处理 |
| Script 块 | CHTL + CHTL JS + JS | 三种编译器协作，字符级切割 |

### 关键代码实现

#### 1. CHTL JS 延续判断
```cpp
bool CHTLUnifiedScanner::isCHTLJSContinuation() {
    // 检查 -> 操作符
    if (source_[pos] == '-' && source_[pos + 1] == '>') {
        return true;
    }
    
    // 检查 . 后跟 CHTL JS 方法
    if (source_[pos] == '.') {
        std::string method = peekIdentifierAt(pos + 1);
        return isCHTLJSMethod(method);
    }
    
    return false;
}
```

#### 2. CHTL JS 扫描
```cpp
void CHTLUnifiedScanner::scanCHTLJS() {
    // 扫描 {{selector}}
    scanSelector();
    
    // 继续扫描 CHTL JS 的延续部分
    while (isCHTLJSContinuation()) {
        if (peek() == '-' && peek(1) == '>') {
            appendToFragment("->"); 
        } else if (peek() == '.') {
            appendToFragment('.');
            scanIdentifier(); // 方法名
            if (peek() == '(') {
                scanBalancedParentheses(); // 参数
            }
        }
    }
}
```

#### 3. Script 块的精细切割
```cpp
void CHTLUnifiedScanner::scanScript() {
    // script { } 的边界由 CHTL 管理
    // 内部根据语法特征动态切换片段类型
    
    while (!isAtEnd() && braceCount > 0) {
        if (matchSequence("{{")) {
            // 切换到 CHTL JS
            endFragment();
            startFragment(FragmentType::CHTL_JS);
            scanCHTLJS();
            endFragment();
            startFragment(FragmentType::JAVASCRIPT);
        }
        // ... 继续扫描 JavaScript
    }
}
```

### 切割示例

输入：
```chtl
script {
    {{.btn}}.listen({
        click: () => {
            console.log("clicked");
            {{#result}}->textContent = "Done";
        }
    });
}
```

输出片段：
1. `CHTL`: `script {`
2. `JAVASCRIPT`: 空白
3. `CHTL_JS`: `{{.btn}}.listen({
        click: () => {`
4. `JAVASCRIPT`: `
            console.log("clicked");
            `
5. `CHTL_JS`: `{{#result}}->textContent`
6. `JAVASCRIPT`: ` = "Done";
        }
    });`
7. `CHTL`: `}`

### 架构优势

1. **完全解耦**：每个编译器只处理纯粹的语法
2. **精确控制**：字符级别的切割保证语法完整性
3. **智能组合**：上下文感知避免过度切割
4. **易于扩展**：新增语法特性只需更新扫描器

### 实现状态

✅ **已完成**：
- 基于上下文的智能扫描器
- 宽严判机制
- CHTL JS 方法识别
- 三种编译器协作的 Script 处理
- 局部/全局样式块区分

🔧 **待优化**：
- 更多 CHTL 语法特性支持（如 `@Var` 等）
- 错误恢复机制
- 性能优化（片段缓存等）

这个统一架构通过精准的扫描器，实现了不同语言的完美分离和协作，为 CHTL 的编译提供了坚实的基础。