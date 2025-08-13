# Token隔离性检查报告

## 检查日期
2024年

## 检查目的
确保各编译器只拥有自身的Token，不插足其他编译器的编译流程。

## Token分配情况

### 1. CHTL编译器（手写）
**负责内容**：CHTL核心语法
**专属Token**：
- HTML元素标识符：`html`, `head`, `body`, `div`, `span`, 等
- CHTL关键字：`text`, `style`, `script`
- 模板关键字：`[Template]`, `[Custom]`, `[Import]`, `[Namespace]`, `[Configuration]`, `[Origin]`
- 属性操作符：`:` (冒号), `=` (等号)
- 结构符号：`{`, `}`, `;`

### 2. CHTL JS编译器（手写）
**负责内容**：CHTL增强的JavaScript语法
**专属Token**：
- CHTL JS指令：`@Element`, `@Style`, `@Var`
- 增强操作符：`->` (链式访问)
- 监听器语法：`on`, `listen`
- 动画关键字：`animate`, `duration`, `easing`, `begin`, `end`, `when`, `at`
- 插值语法：`{{`, `}}`
- 特殊关键字：`this`

### 3. CSS编译器（ANTLR）
**负责内容**：原生CSS语法
**专属Token**：
- CSS选择器：`.class`, `#id`, `:pseudo-class`, `::pseudo-element`
- CSS属性：`color`, `background-color`, `font-size`, 等
- CSS值：颜色值、尺寸单位、函数
- CSS规则：`@media`, `@keyframes`, `@import`
- CSS注释：`/* */`

### 4. JavaScript编译器（ANTLR）
**负责内容**：原生JavaScript语法
**专属Token**：
- JS关键字：`var`, `let`, `const`, `function`, `if`, `else`, `for`, `while`
- JS操作符：`+`, `-`, `*`, `/`, `===`, `!==`, `&&`, `||`
- JS字面量：字符串、数字、布尔值、null、undefined
- JS语法：`()`, `[]`, `.` (成员访问)
- JS注释：`//`, `/* */`

## 潜在冲突分析

### 1. 共享Token
以下Token在多个编译器中出现，但用途明确：
- `{`, `}` - 所有编译器都使用，但在各自上下文中含义明确
- `;` - CHTL和JS都使用，但语境不同
- `//`, `/* */` - 注释在各语言中都支持

### 2. 已解决的冲突
- **`@Style`冲突**：
  - 在CHTL中：`[Template] @Style` 用于样式模板
  - 在CHTL JS中：`@Style` 用于样式组访问
  - 解决方案：通过上下文区分，CHTL处理`[Template]`前缀的，CHTL JS处理独立的

- **`.`操作符**：
  - 在CSS中：类选择器
  - 在JS中：成员访问
  - 在CHTL JS中：建议使用`->`替代
  - 解决方案：扫描器根据上下文（style块 vs script块）分配

## 扫描器优化建议

### 1. 精准切割规则
扫描器应该根据以下规则精准切割代码片段：

```
CHTL片段：
- 顶层HTML元素和结构
- [Template], [Custom], [Import]等声明
- text { } 块

CHTL JS片段：
- script块中的@Element, @Style, @Var使用
- ->链式访问
- on/listen事件绑定
- animate动画定义

CSS片段：
- style { } 块内的内容
- [Origin] @Style块

JavaScript片段：
- script { } 块内的原生JS代码
- [Origin] @JavaScript块
```

### 2. 上下文感知
扫描器需要维护当前上下文状态：
- 在style块内 → CSS编译器
- 在script块内 → 检查是否有CHTL JS特征
  - 有@Element等 → CHTL JS编译器
  - 纯JS代码 → JavaScript编译器
- 其他位置 → CHTL编译器

### 3. 边界处理
明确的边界标记：
- `style {` ... `}` → CSS边界
- `script {` ... `}` → JS/CHTL JS边界
- `[Origin]` → 原始代码边界
- `text {` ... `}` → 文本内容边界

## 结论

当前的Token分配基本合理，各编译器的职责明确：
1. **CHTL编译器**：负责结构和模板系统
2. **CHTL JS编译器**：负责增强的JavaScript特性
3. **CSS编译器**：负责样式处理
4. **JavaScript编译器**：负责原生JS代码

通过扫描器的精准切割和上下文感知，可以确保各编译器不会互相干扰，各司其职。