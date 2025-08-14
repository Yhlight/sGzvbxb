# CHTL - C++ Hypertext Language

## 简介

CHTL（C++ Hypertext Language）是一种基于 C++ 的超文本语言，专为现代 Web 开发设计。它提供了编译时优化、语言级组件化和强大的模板系统，让 Web 开发更加高效和优雅。

## 特性

### 核心特性
- ✅ **完整的 HTML 支持**：支持所有 HTML 元素和属性
- ✅ **模板系统**：样式组模板、元素模板、变量组模板
- ✅ **自定义系统**：高度灵活的自定义组件支持
- ✅ **局部样式**：自动作用域隔离，避免样式冲突
- ✅ **CHTL JS**：增强的 JavaScript 语法，简化 DOM 操作
- ✅ **模块化**：完整的导入/导出系统
- ✅ **命名空间**：避免名称冲突

### 语法特性
- 无引号字面量
- 链式访问操作符 `->`
- DOM 选择器 `{{selector}}`
- 事件监听简化语法
- 动画 API 集成

## 快速开始

### 安装

```bash
git clone https://github.com/yourusername/chtl.git
cd chtl
mkdir build && cd build
cmake ..
make -j4
```

### 第一个 CHTL 程序

```chtl
// hello.chtl
body {
    h1 {
        text "Hello, CHTL!"
        style {
            color: "blue";
            text-align: "center";
        }
    }
    
    button {
        text "Click me"
        script {
            {{button}}->listen({
                click: () => {
                    alert("Hello from CHTL!");
                }
            });
        }
    }
}
```

编译：
```bash
chtl hello.chtl -o hello.html
```

## 语法示例

### 模板系统

```chtl
[Template] @Style PrimaryButton {
    background-color: "#007bff";
    color: "white";
    padding: "10px 20px";
    border-radius: "4px";
}

[Template] @Element Card {
    div {
        style {
            border: "1px solid #ddd";
            padding: "20px";
        }
        slot;  // 内容插槽
    }
}

// 使用模板
body {
    @Element Card {
        h2 { text "Card Title" }
        p { text "Card content" }
    }
    
    button {
        style {
            @Style PrimaryButton;
        }
        text "Submit"
    }
}
```

### CHTL JS

```chtl
script {
    // DOM 选择
    {{#myButton}}->listen({
        click: () => {
            {{.result}}->textContent = "Clicked!";
            {{.result}}->animate({
                opacity: [0, 1],
                duration: 300
            });
        }
    });
}
```

### 局部样式

```chtl
div {
    style {
        background: "white";
        
        .highlight {
            color: "red";
            font-weight: "bold";
        }
        
        #title {
            font-size: "24px";
        }
    }
    
    h1 {
        id: "title";
        text "Title"
    }
    
    p {
        class: "highlight";
        text "Important text"
    }
}
```

## 编译器选项

- `--enhanced`：使用增强解析器（支持完整 CHTL 语法）
- `-o <file>`：指定输出文件
- `--debug`：启用调试输出
- `--optimize`：启用优化

## 项目结构

```
chtl/
├── src/
│   ├── chtl/           # 核心 CHTL 实现
│   ├── parser/         # 解析器
│   ├── compiler/       # 编译器
│   └── optimizer/      # 优化器
├── test/               # 测试文件
├── docs/               # 文档
└── examples/           # 示例代码
```

## 贡献

欢迎贡献代码！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详情。

## 许可证

本项目采用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

## 相关链接

- [CHTL 语法文档](docs/CHTL语法文档.md)
- [API 文档](docs/api.md)
- [示例项目](examples/)

## 路线图

- [x] 核心解析器
- [x] 代码生成器
- [x] 模板系统
- [x] CHTL JS
- [ ] IDE 插件
- [ ] 在线编译器
- [ ] 包管理器