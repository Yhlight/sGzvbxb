# 编译器职责更新说明

## 更新日期
2024年

## 背景
根据用户澄清，需要更正对各编译器职责的理解，特别是关于`@Element`, `@Style`, `@Var`的归属。

## 核心原则

### 1. CHTL特征的归属
- `@Element`, `@Style`, `@Var` 是**CHTL的核心特征**
- 无论它们出现在何处（包括script块内），都属于CHTL的语法
- 这些不是CHTL JS独有的特征

### 2. style块的处理原则
- **全局style块**（顶层）→ CSS编译器处理
- **局部style块**（元素内部）→ CHTL编译器处理
- 局部style是CHTL对CSS的增强功能

### 3. script块的处理原则
- script块是**CHTL JS和原生JS的共同作用域**
- 需要根据具体语法特征来区分：
  - CHTL JS特征：`->`, `on`, `listen`, `animate`等
  - 原生JS：标准JavaScript语法

## 扫描器更新建议

### 1. 片段类型判断逻辑

```cpp
// 伪代码
if (在全局style块内) {
    return FragmentType::CSS;
} else if (在局部style块内) {
    return FragmentType::CHTL;  // 局部style由CHTL处理
} else if (在script块内) {
    if (包含CHTL_JS特征) {  // ->, on, listen, animate等
        return FragmentType::CHTL_JS;
    } else {
        return FragmentType::JS;
    }
} else {
    return FragmentType::CHTL;  // 默认为CHTL
}
```

### 2. CHTL JS特征识别
CHTL JS编译器应该只识别以下增强语法：
- `->` 链式访问操作符
- `on` 事件绑定语法
- `listen` 监听器语法
- `animate` 动画API
- `{{` `}}` 插值语法
- 其他CHTL JS特有的增强功能

注意：`@Element`, `@Style`, `@Var`不应作为判断CHTL JS的标志。

### 3. 混合处理策略
在script块内可能同时存在：
- CHTL特征（`@Element`等）
- CHTL JS特征（`->`等）
- 原生JS代码

建议处理方式：
1. 扫描器识别到script块后，进一步分析内容
2. 如果包含CHTL JS特征，标记为CHTL_JS片段
3. CHTL JS编译器处理时，遇到`@Element`等CHTL特征时，可以：
   - 保留原样传递给后续处理
   - 或调用CHTL编译器的相关功能

## 实现影响

### 1. CHTLUnifiedScanner
需要更新`scanScriptBlock`方法，不再将`@Element`等作为CHTL JS的标志。

### 2. 编译器包装器
- `CHTLCompilerWrapper`：需要能处理局部style
- `CHTLJSCompilerWrapper`：只处理CHTL JS增强语法
- `CSSCompilerWrapper`：只处理全局style
- `JSCompilerWrapper`：处理原生JS

### 3. 测试用例
需要更新测试用例以反映新的职责划分：
- 测试局部style由CHTL编译器处理
- 测试`@Element`等在script中仍是CHTL特征
- 测试CHTL JS只处理增强语法

## 总结
这次更新澄清了CHTL项目中各编译器的明确职责边界，确保了：
1. CHTL的核心特征始终由CHTL管理
2. style的处理根据位置决定
3. script块内的代码根据具体语法特征分配给合适的编译器