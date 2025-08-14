# CHTL 官方模块导入系统

## 概述

CHTL 支持使用 `chtl::` 前缀来明确指定导入官方模块，有效解决了用户自定义模块与官方模块之间的命名冲突问题。

## 语法

### 1. 官方模块导入

使用 `chtl::` 前缀明确指定导入官方模块：

```chtl
[Import] @Chtl from "chtl::ModuleName"
[Import] @CJmod from "chtl::LibraryName"
```

### 2. 普通模块导入

不带前缀的导入会按照配置的优先级搜索：

```chtl
[Import] @Chtl from "ModuleName"  // 按优先级搜索：官方 → 本地module → 当前目录
```

## 功能特性

### 1. 明确的官方模块引用

```chtl
// 明确导入官方 Router 模块
[Import] @Chtl from "chtl::Router"

// 可能导入用户自定义的 Router 模块
[Import] @Chtl from "Router"
```

### 2. 官方子模块支持

```chtl
// 导入官方模块的子模块
[Import] @Chtl from "chtl::UI.Components.Button"
[Import] @Chtl from "chtl::Framework/Core/Base"
```

### 3. 官方模块通配符

```chtl
// 导入所有官方 UI 模块
[Import] @Chtl from "chtl::UI.*"

// 导入官方组件目录下的所有 .cmod 文件
[Import] @Chtl from "chtl::Components/*.cmod"
```

### 4. 搜索行为

#### 带 `chtl::` 前缀
- **只在官方模块目录搜索**
- 不会搜索用户目录
- 找不到时直接报错

#### 不带前缀
- 按照配置的优先级搜索
- 默认顺序：官方目录 → 本地 module 目录 → 当前目录
- 可通过配置修改搜索优先级

## 使用场景

### 1. 避免命名冲突

当用户创建了与官方模块同名的模块时：

```chtl
// 项目结构
// ./module/Router.cmod        (用户的 Router)
// ./official/Router.cmod      (官方的 Router)

[Import] @Chtl from "Router"        // 根据优先级，可能是官方或用户模块
[Import] @Chtl from "chtl::Router"  // 明确使用官方 Router
```

### 2. 确保使用官方版本

在团队开发中，确保所有人使用相同的官方模块版本：

```chtl
// 强制使用官方 UI 组件库
[Import] @Chtl from "chtl::UI.Components"
[Import] @Chtl from "chtl::UI.Layouts"
[Import] @Chtl from "chtl::UI.Themes"
```

### 3. 官方模块集合导入

快速导入一组相关的官方模块：

```chtl
// 导入所有官方动画相关模块
[Import] @Chtl from "chtl::Animation.*"

// 导入所有官方工具模块
[Import] @CJmod from "chtl::Utils/*.cjmod"
```

## 实现细节

### 路径解析

1. **检测前缀**：检查路径是否以 `chtl::` 开头
2. **提取模块名**：移除 `chtl::` 前缀
3. **定向搜索**：只在官方模块目录中搜索

### 文件搜索优先级

对于 CHTL 模块：
1. 优先搜索 `.cmod` 文件
2. 其次搜索 `.chtl` 文件

对于 CJMOD：
- 只搜索 `.cjmod` 文件

### 错误处理

- 官方模块不存在时，报告明确的错误信息
- 提示可能的模块名拼写错误
- 建议查看官方模块列表

## 配置选项

可以通过配置文件设置官方模块目录：

```json
{
  "import": {
    "officialModulePath": "/path/to/official/modules",
    "searchOfficialFirst": true
  }
}
```

## 最佳实践

1. **明确意图**：当确定要使用官方模块时，始终使用 `chtl::` 前缀
2. **避免同名**：尽量避免创建与官方模块同名的用户模块
3. **文档化**：在项目文档中说明使用了哪些官方模块
4. **版本控制**：记录所依赖的官方模块版本

## 示例

```chtl
// 导入官方核心模块
[Import] @Chtl from "chtl::Core"
[Import] @Chtl from "chtl::Framework"

// 导入官方 UI 组件
[Import] @Chtl from "chtl::UI.Button"
[Import] @Chtl from "chtl::UI.Form"

// 导入官方工具库
[Import] @CJmod from "chtl::utils"
[Import] @CJmod from "chtl::animations"

// 使用导入的官方组件
body {
    @Element Button from UI.Button {
        text {
            "Official Button"
        }
        onclick: "handleClick"
    }
    
    script {
        // 使用官方动画库
        animate({
            target: "button",
            property: "opacity",
            from: 0,
            to: 1,
            duration: 500
        });
    }
}
```