# Token隔离性检查报告

## 检查日期
2024年

## 检查目的
确保各编译器只拥有自身的Token，不插足其他编译器的编译流程。

## Token分配情况

### 1. CHTL编译器（手写）
**负责内容**：CHTL核心语法、局部样式、模板系统
**专属Token**：
- HTML元素标识符：`html`, `head`, `body`, `div`, `span`, 等
- CHTL关键字：`text`, `style`（局部）, `script`
- 模板关键字：`[Template]`, `[Custom]`, `[Import]`, `[Namespace]`, `[Configuration]`, `[Origin]`
- CHTL特征：`@Element`, `@Style`, `@Var`（无论在何处都是CHTL特征）
- 属性操作符：`:` (冒号), `=` (等号)
- 结构符号：`{`, `}`, `;`

### 2. CHTL JS编译器（手写）
**负责内容**：script块内的增强JavaScript语法
**专属Token**：
- 增强操作符：`->` (链式访问)
- 监听器语法：`on`, `listen`
- 动画关键字：`animate`, `duration`, `easing`, `begin`, `end`, `when`, `at`
- 插值语法：`{{`, `}}`
- 特殊关键字：`this`
- 注：在script中使用`@Element`, `@Style`, `@Var`时，这些仍是CHTL特征，但在CHTL JS上下文中使用

### 3. CSS编译器（ANTLR）
**负责内容**：全局style块的原生CSS语法
**专属Token**：
- CSS选择器：`.class`, `#id`, `:pseudo-class`, `::pseudo-element`
- CSS属性：`color`, `background-color`, `font-size`, 等
- CSS值：颜色值、尺寸单位、函数
- CSS规则：`@media`, `@keyframes`, `@import`
- CSS注释：`/* */`
- 注：仅处理全局style块，局部style由CHTL编译器处理

### 4. JavaScript编译器（ANTLR）
**负责内容**：script块内的原生JavaScript语法
**专属Token**：
- JS关键字：`var`, `let`, `const`, `function`, `if`, `else`, `for`, `while`
- JS操作符：`+`, `-`, `*`, `/`, `===`, `!==`, `&&`, `||`
- JS字面量：字符串、数字、布尔值、null、undefined
- JS语法：`()`, `[]`, `.` (成员访问)
- JS注释：`//`, `/* */`
- 注：与CHTL JS编译器共享script块作用域

## 潜在冲突分析

### 1. 共享Token
以下Token在多个编译器中出现，但用途明确：
- `{`, `}` - 所有编译器都使用，但在各自上下文中含义明确
- `;` - CHTL和JS都使用，但语境不同
- `//`, `/* */` - 注释在各语言中都支持

### 2. 重要区分
- **`@Element`, `@Style`, `@Var`的归属**：
  - 这些都是CHTL的核心特征，无论出现在何处
  - 在script块中使用时，是CHTL特征在JS上下文中的应用
  - 不属于CHTL JS独有的语法

- **style块的处理**：
  - 全局style块（顶层）→ CSS编译器
  - 局部style块（元素内部）→ CHTL编译器
  - 这是CHTL对style的增强功能

- **script块的共享**：
  - script块是CHTL JS和原生JS的共同作用域
  - CHTL JS处理增强语法（`->`, `on`, `listen`, `animate`）
  - JavaScript编译器处理原生JS语法

## 扫描器优化建议

### 1. 精准切割规则
扫描器应该根据以下规则精准切割代码片段：

```
CHTL片段：
- 顶层HTML元素和结构
- [Template], [Custom], [Import]等声明
- text { } 块
- 局部style块（元素内的style）
- @Element, @Style, @Var的所有使用

CHTL JS片段（script块内）：
- ->链式访问
- on/listen事件绑定
- animate动画定义
- 其他CHTL JS增强语法

CSS片段：
- 全局style { } 块的内容
- [Origin] @Style块

JavaScript片段（script块内）：
- 原生JS代码
- [Origin] @JavaScript块
```

### 2. 上下文感知
扫描器需要维护当前上下文状态：
- 全局style块 → CSS编译器
- 局部style块（元素内） → CHTL编译器
- script块内：
  - 有CHTL JS特征（`->`, `on`, `listen`, `animate`） → CHTL JS编译器
  - 纯JS代码 → JavaScript编译器
  - 注意：`@Element`, `@Style`, `@Var`仍属于CHTL特征
- 其他位置 → CHTL编译器

### 3. 边界处理
明确的边界标记：
- `style {` ... `}` → CSS边界
- `script {` ... `}` → JS/CHTL JS边界
- `[Origin]` → 原始代码边界
- `text {` ... `}` → 文本内容边界

## 结论

根据更正后的理解，各编译器的职责更加明确：

1. **CHTL编译器**：
   - 负责所有CHTL核心特征（`@Element`, `@Style`, `@Var`）
   - 处理HTML结构、模板系统
   - 处理局部style块（CHTL的增强功能）

2. **CHTL JS编译器**：
   - 仅负责script块内的增强JS语法
   - 处理`->`, `on`, `listen`, `animate`等特性
   - 与原生JS编译器共享script作用域

3. **CSS编译器**：
   - 仅处理全局style块
   - 负责原生CSS语法

4. **JavaScript编译器**：
   - 处理script块内的原生JS代码
   - 与CHTL JS编译器共享script作用域

这种划分确保了：
- CHTL的核心特征始终由CHTL编译器处理
- style的处理根据位置（全局/局部）分配
- script块内JS和CHTL JS可以协同工作