# CHTL 增强导入系统

## 概述

CHTL 的增强导入系统提供了强大的模块管理功能，包括路径规范化、循环依赖检测、重复导入防护等特性。

## 主要特性

### 1. 路径规范化
- 自动处理相同路径的不同表达方式（如 `./file.chtl`、`file.chtl`、`../dir/file.chtl`）
- 支持点号（`.`）和斜杠（`/`）的互换：`Chtholly.Space` 等价于 `Chtholly/Space`
- 规范化后的路径用于重复导入检测

### 2. 循环依赖检测
- 实时检测导入链中的循环依赖
- 提供详细的循环路径信息
- 防止无限递归导入

### 3. 重复导入防护
- 使用规范化路径检测重复导入
- 相同文件只会被导入一次
- 提高编译效率

### 4. 资源文件导入（@Html, @Style, @JavaScript）

#### 无 as 子句
```chtl
[Import] @Html from "header.html"       // 直接跳过，不创建节点
[Import] @Style from "theme.css"        // 直接跳过，不创建节点
[Import] @JavaScript from "utils.js"    // 直接跳过，不创建节点
```

#### 带 as 子句
```chtl
[Import] @Html from "header.html" as headerTemplate
[Import] @Style from "theme.css" as themeStyles
[Import] @JavaScript from "utils.js" as utilsScript

// 使用导入的原始嵌入
body {
    [Origin] @Html headerTemplate;
    [Origin] @Style themeStyles;
    script {
        [Origin] @JavaScript utilsScript;
    }
}
```

#### 搜索规则
- 只在当前编译文件所在目录搜索（不递归）
- 如果路径是文件名（不带后缀），根据类型添加对应扩展名：
  - @Html → .html
  - @Style → .css
  - @JavaScript → .js
- 如果路径是具体文件名（带后缀），直接搜索该文件
- 如果路径是文件夹，报错

### 5. CHTL 模块导入（@Chtl）

#### 搜索优先级
1. 官方模块目录（`module/`）
2. 当前目录的 `module/` 文件夹
3. 当前目录

#### 文件优先级
- 优先搜索 `.cmod` 文件
- 其次搜索 `.chtl` 文件

#### 示例
```chtl
// 模块名导入
[Import] @Chtl from "Chtholly"              // 搜索 Chtholly.cmod 或 Chtholly.chtl

// 子模块导入（支持点号和斜杠）
[Import] @Chtl from "Chtholly.Space"        // 等价于 Chtholly/Space
[Import] @Chtl from "Chtholly/Timeline"

// 通配符导入
[Import] @Chtl from "modules/*"             // 导入所有 .cmod 和 .chtl
[Import] @Chtl from "modules/*.cmod"        // 只导入 .cmod
[Import] @Chtl from "components.*.chtl"     // 点号形式的通配符

// 具体路径导入
[Import] @Chtl from "/absolute/path/to/module.cmod"
[Import] @Chtl from "./relative/module.chtl"
```

### 6. CJMOD 导入（@CJmod）

#### 搜索优先级
1. 官方模块目录
2. 当前目录的 `module/` 文件夹  
3. 当前目录

#### 示例
```chtl
[Import] @CJmod from "animations"           // 搜索 animations.cjmod
[Import] @CJmod from "animations.cjmod"     // 具体文件名
[Import] @CJmod from "/path/to/module.cjmod"
```

### 7. 错误处理

#### 文件夹路径错误
```chtl
[Import] @Html from "styles/"    // 错误：资源导入不能是文件夹
[Import] @Chtl from "modules/"   // 错误：需要使用通配符 modules/*
```

#### 循环依赖错误
```
Circular dependency detected: a.chtl -> b.chtl -> c.chtl -> a.chtl
```

## 实现细节

### 类结构

1. **PathNormalizer** - 路径规范化工具
   - 处理点号到斜杠的转换
   - 解析相对路径
   - 识别文件扩展名

2. **FileSearcherEnhanced** - 文件搜索器
   - 根据导入类型搜索文件
   - 支持多路径搜索
   - 处理通配符模式

3. **CircularDependencyDetectorImpl** - 循环依赖检测器
   - 维护依赖图
   - DFS 检测循环
   - 生成循环路径

4. **ImportManagerEnhanced** - 增强的导入管理器
   - 协调整个导入过程
   - 管理路径缓存
   - 处理不同类型的导入

5. **ImportProcessorEnhanced** - 导入处理器
   - 解析导入语句
   - 验证导入声明
   - 处理子模块导入

### 线程安全
- 路径缓存使用互斥锁保护
- 循环依赖检测器线程安全
- 支持并发编译场景

### 性能优化
- 规范化路径缓存
- 文件内容缓存
- 避免重复文件系统访问