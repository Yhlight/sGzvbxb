# CHTL用户指南

## 目录

1. [快速开始](#快速开始)
2. [基础语法](#基础语法)
3. [模板系统](#模板系统)
4. [自定义系统](#自定义系统)
5. [CHTL JS特性](#chtl-js特性)
6. [模块系统](#模块系统)
7. [最佳实践](#最佳实践)
8. [调试技巧](#调试技巧)

## 快速开始

### 安装

```bash
# 克隆项目
git clone https://github.com/chtl-lang/chtl.git
cd chtl

# 设置依赖
./setup_dependencies.sh

# 构建项目
mkdir build && cd build
cmake ..
make -j$(nproc)

# 安装（可选）
sudo make install
```

### 第一个CHTL文件

创建 `hello.chtl`：

```chtl
html
{
    head
    {
        title { Hello CHTL }
    }
    
    body
    {
        h1 { text { 欢迎使用CHTL！ } }
        p { text { CHTL让HTML编写更加优雅 } }
    }
}
```

### 编译

```bash
chtl compile hello.chtl -o hello.html
```

## 基础语法

### 元素声明

CHTL使用大括号语法代替尖括号：

```chtl
// CHTL
div
{
    class: container;
    id: main;
    
    p { text { 这是一个段落 } }
}

// 等价的HTML
<div class="container" id="main">
    <p>这是一个段落</p>
</div>
```

### 属性

属性使用 `key: value` 语法：

```chtl
a
{
    href: https://example.com;
    target: _blank;
    class: link external;
    text { 访问示例网站 }
}
```

### 无修饰字面量

CHTL支持无引号的字符串（类似CSS）：

```chtl
div
{
    class: my-class another-class;  // 无需引号
    data-value: 123;                // 数字
    style
    {
        color: #333;                // 颜色值
        font-size: 16px;            // 带单位的值
    }
}
```

### 注释

支持三种注释方式：

```chtl
// 单行注释

/* 
   多行注释
   可以跨越多行
*/

-- HTML风格的注释
```

### text块

用于定义元素的文本内容：

```chtl
p
{
    text 
    { 
        这是一个段落的文本内容。
        可以包含多行。
    }
}

// 简写形式
p { text { 单行文本 } }
```

## 模板系统

### 变量组模板

定义可重用的变量集合：

```chtl
[Template] @Var Colors
{
    primary: #2196F3;
    secondary: #FF4081;
    success: #4CAF50;
    warning: #FF9800;
    error: #F44336;
}

// 使用变量
button
{
    style
    {
        background-color: Colors(primary);
        color: white;
    }
}
```

### 样式组模板

定义可重用的样式集合：

```chtl
[Template] @Style CardStyle
{
    padding: 20px;
    border-radius: 8px;
    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    background: white;
}

// 使用样式组
div
{
    class: card;
    style
    {
        @Style CardStyle;  // 继承所有样式
        margin-bottom: 20px;  // 添加额外样式
    }
}
```

### 元素模板

定义可重用的元素结构：

```chtl
[Template] @Element Button
{
    button
    {
        class: btn {{variant}};
        style
        {
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }
        
        text { {{label}} }
    }
}

// 使用元素模板
@Element Button(label: "点击我", variant: "primary");
```

### 模板继承

模板可以继承其他同类型模板：

```chtl
[Template] @Style BaseButton
{
    padding: 10px 20px;
    border: none;
    cursor: pointer;
}

[Template] @Style PrimaryButton
{
    @Style BaseButton;  // 继承
    background: #2196F3;
    color: white;
}
```

## 自定义系统

### 自定义元素

创建更灵活的元素定义：

```chtl
[Custom] @Element Modal
{
    div(class: modal-overlay)
    {
        div(class: modal-content)
        {
            div(class: modal-header)
            {
                h2 { text { {{title}} } }
                button(class: close-btn) { text { × } }
            }
            
            div(class: modal-body)
            {
                {{content}}
            }
            
            div(class: modal-footer)
            {
                {{footer}}
            }
        }
    }
}

// 使用自定义元素
@Element Modal
{
    title: "确认操作";
    content: p { text { 您确定要执行此操作吗？ } }
    footer: {
        button { text { 取消 } }
        button { text { 确认 } }
    }
}
```

### 自定义操作

使用 `insert` 和 `delete` 操作：

```chtl
[Custom] @Element List
{
    ul(class: custom-list)
    {
        {{items}}
    }
    
    // 插入操作
    insert before .custom-list
    {
        h3 { text { {{title}} } }
    }
}
```

## CHTL JS特性

### DOM选择增强

使用 `{{selector}}` 语法：

```chtl
script
{
    // 选择单个元素
    const button = {{#submitBtn}};
    const form = {{.login-form}};
    
    // 选择多个元素
    const items = {{.list-item}};
    
    // 带索引选择
    const firstItem = {{.list-item[0]}};
}
```

### 链式访问

使用 `->` 替代 `.`：

```chtl
script
{
    const input = {{#username}};
    
    // 链式调用
    input->value = "新值";
    input->classList->add("active");
    input->style->display = "block";
}
```

### 事件监听增强

使用 `listen` 方法：

```chtl
script
{
    const button = {{#myButton}};
    
    // 单个事件
    button.listen({
        click: (e) => {
            console.log("按钮被点击");
        }
    });
    
    // 多个事件
    const form = {{#myForm}};
    form.listen({
        submit: (e) => {
            e.preventDefault();
            // 处理提交
        },
        reset: (e) => {
            console.log("表单重置");
        }
    });
}
```

### 动画API

使用 `animate` 函数：

```chtl
script
{
    const box = {{.box}};
    
    animate({
        target: box,
        duration: 500,
        from: {
            opacity: 0,
            transform: 'translateY(-20px)'
        },
        to: {
            opacity: 1,
            transform: 'translateY(0)'
        },
        easing: 'ease-out',
        onComplete: () => {
            console.log("动画完成");
        }
    });
}
```

## 模块系统

### CMOD（组件模块）

创建可重用的组件：

`components/Button.chtl`:
```chtl
[Export] @Element Button
{
    button
    {
        class: custom-button {{variant}};
        text { {{text}} }
    }
}

[Export] @Style ButtonStyles
{
    padding: 10px 20px;
    border-radius: 4px;
}
```

使用组件：
```chtl
[Import] @Element Button from ./components/Button.chtl;
[Import] @Style ButtonStyles from ./components/Button.chtl;

body
{
    @Element Button(text: "点击", variant: "primary");
}
```

### 命名空间

避免命名冲突：

```chtl
[Namespace] UI
{
    [Template] @Element Button { /* ... */ }
    [Template] @Style Theme { /* ... */ }
}

// 使用命名空间
body
{
    UI::@Element Button();
}
```

## 最佳实践

### 1. 组织结构

```
project/
├── src/
│   ├── pages/
│   │   ├── index.chtl
│   │   └── about.chtl
│   ├── components/
│   │   ├── Header.chtl
│   │   └── Footer.chtl
│   ├── styles/
│   │   └── theme.chtl
│   └── scripts/
│       └── main.chtl
├── build/
└── chtl.config.json
```

### 2. 命名约定

- 元素模板：PascalCase（如 `UserCard`）
- 样式组：PascalCase + Style（如 `CardStyle`）
- 变量组：描述性名称（如 `ThemeColors`）
- ID：kebab-case（如 `user-profile`）
- 类名：kebab-case（如 `nav-item`）

### 3. 性能优化

- 使用模板减少重复代码
- 合理使用自定义元素
- 避免深层嵌套
- 利用编译时优化

### 4. 可维护性

- 模块化组件设计
- 清晰的文件组织
- 充分的注释
- 一致的代码风格

## 调试技巧

### 1. 编译错误

```bash
# 详细错误信息
chtl compile file.chtl -v

# 验证语法
chtl validate file.chtl
```

### 2. Source Map

```bash
# 生成source map
chtl compile file.chtl -o output.html --source-map
```

### 3. 开发模式

```bash
# 监视文件变化
chtl watch src/ -o build/

# 开发服务器
chtl serve --port 3000
```

### 4. VS Code集成

安装CHTL语言支持插件：
- 语法高亮
- 自动完成
- 错误提示
- 格式化

## 常见问题

### Q: CHTL与HTML有什么区别？

A: CHTL提供了更结构化的语法、模板系统、增强的JavaScript特性，同时保持与HTML的兼容性。

### Q: 可以在现有项目中使用CHTL吗？

A: 是的，CHTL可以逐步集成到现有项目中，支持混合使用。

### Q: CHTL的性能如何？

A: CHTL在编译时进行优化，生成的HTML/CSS/JS经过优化，运行时性能与原生相当。

### Q: 如何贡献代码？

A: 欢迎提交Issue和Pull Request，请遵循项目的贡献指南。

---

更多信息请访问：[CHTL官方网站](https://chtl-lang.org)