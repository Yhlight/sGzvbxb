# ANTLR生成的文件列表

CHTL项目使用ANTLR 4.13.1生成了以下解析器文件：

## 1. CHTL语法解析器 (`/workspace/generated/chtl/grammars/`)

基于 `grammars/CHTL.g4` 生成：

- **词法分析器 (Lexer)**:
  - `CHTLLexer.h` - 词法分析器头文件
  - `CHTLLexer.cpp` - 词法分析器实现
  - `CHTLLexer.interp` - 词法分析器解释文件
  - `CHTLLexer.tokens` - Token定义文件

- **语法分析器 (Parser)**:
  - `CHTLParser.h` - 语法分析器头文件
  - `CHTLParser.cpp` - 语法分析器实现 (223KB)
  - `CHTL.interp` - 语法解释文件
  - `CHTL.tokens` - Token定义文件

- **访问者模式 (Visitor Pattern)**:
  - `CHTLVisitor.h` - 访问者接口
  - `CHTLVisitor.cpp` - 访问者基础实现
  - `CHTLBaseVisitor.h` - 基础访问者类
  - `CHTLBaseVisitor.cpp` - 基础访问者实现

- **监听器模式 (Listener Pattern)**:
  - `CHTLListener.h` - 监听器接口
  - `CHTLListener.cpp` - 监听器基础实现
  - `CHTLBaseListener.h` - 基础监听器类
  - `CHTLBaseListener.cpp` - 基础监听器实现

## 2. CSS解析器 (`/workspace/generated/css/grammars/`)

基于 `grammars/CSS.g4` 生成：

- **词法分析器**: CSSLexer.h/.cpp
- **语法分析器**: CSSParser.h/.cpp (200KB)
- **访问者模式**: CSSVisitor.h/.cpp, CSSBaseVisitor.h/.cpp
- **监听器模式**: CSSListener.h/.cpp, CSSBaseListener.h/.cpp

## 3. JavaScript解析器 (`/workspace/generated/javascript/grammars/`)

基于 `grammars/JavaScript.g4` 生成：

- **词法分析器**: JavaScriptLexer.h/.cpp
- **语法分析器**: JavaScriptParser.h/.cpp
- **访问者模式**: JavaScriptVisitor.h/.cpp, JavaScriptBaseVisitor.h/.cpp
- **监听器模式**: JavaScriptListener.h/.cpp, JavaScriptBaseListener.h/.cpp

## 4. CHTL JavaScript解析器 (`/workspace/generated/chtljs/grammars/`)

基于 `grammars/CHTLJavaScript.g4` 生成：

- **词法分析器**: CHTLJavaScriptLexer.h/.cpp
- **语法分析器**: CHTLJavaScriptParser.h/.cpp
- **访问者模式**: CHTLJavaScriptVisitor.h/.cpp, CHTLJavaScriptBaseVisitor.h/.cpp
- **监听器模式**: CHTLJavaScriptListener.h/.cpp, CHTLJavaScriptBaseListener.h/.cpp

## 5. 扫描器 (`/workspace/generated/scanner/grammars/`)

基于 `grammars/Scanner.g4` 生成：

- **词法分析器**: ScannerLexer.h/.cpp
- **语法分析器**: ScannerParser.h/.cpp
- **访问者模式**: ScannerVisitor.h/.cpp, ScannerBaseVisitor.h/.cpp
- **监听器模式**: ScannerListener.h/.cpp, ScannerBaseListener.h/.cpp

## 生成命令

这些文件是通过运行以下脚本生成的：

```bash
./setup_dependencies.sh
./generate_parsers.sh
```

生成过程使用的ANTLR命令示例：

```bash
java -jar tools/antlr-4.13.1-complete.jar \
  -Dlanguage=Cpp \
  -visitor \
  -o generated/chtl/grammars \
  grammars/CHTL.g4
```

## 文件统计

总共生成了 **56个** C++源文件和头文件，为CHTL编译器提供了完整的解析能力。

## 注意事项

1. 这些文件是自动生成的，不应该手动编辑
2. 如果修改了`.g4`语法文件，需要重新运行生成脚本
3. 生成的文件已经包含在编译系统中，CMakeLists.txt会自动找到并编译它们