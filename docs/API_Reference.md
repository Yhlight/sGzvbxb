# CHTL API Reference

## Table of Contents

1. [Scanner API](#scanner-api)
2. [Parser API](#parser-api)
3. [Compiler API](#compiler-api)
4. [Code Generation API](#code-generation-api)
5. [Error Handling API](#error-handling-api)
6. [Module System API](#module-system-api)

## Scanner API

### CHTLUnifiedScanner

The unified scanner is responsible for intelligently splitting CHTL source code into fragments for processing by different compilers.

```cpp
namespace chtl::scanner {

class CHTLUnifiedScanner {
public:
    // Constructor
    CHTLUnifiedScanner();
    
    // Main scanning method
    std::vector<CodeFragment> scan(const std::string& source);
    
    // Configuration
    void setDebugMode(bool debug);
};

// Fragment types
enum class FragmentType {
    CHTL,       // CHTL syntax
    CHTL_JS,    // CHTL JavaScript extensions
    CSS,        // Standard CSS
    JAVASCRIPT  // Standard JavaScript
};

// Code fragment structure
struct CodeFragment {
    FragmentType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
};

}
```

#### Usage Example

```cpp
#include <chtl/scanner/CHTLUnifiedScanner.h>

using namespace chtl::scanner;

CHTLUnifiedScanner scanner;
scanner.setDebugMode(true);

std::string source = R"(
body {
    style {
        color: blue;
    }
    text "Hello"
}
)";

auto fragments = scanner.scan(source);
for (const auto& fragment : fragments) {
    std::cout << "Type: " << static_cast<int>(fragment.type) 
              << ", Content: " << fragment.content << std::endl;
}
```

## Parser API

### CHTLParserEnhanced

The enhanced parser supports the full CHTL syntax including templates, custom elements, and advanced features.

```cpp
namespace chtl::parser::standalone {

class CHTLParserEnhanced {
public:
    // Parse CHTL source code
    std::unique_ptr<ParseTree> parse(const std::string& source);
    
    // Error handling
    bool hasErrors() const;
    std::vector<ParseError> getErrors() const;
    
    // Configuration
    void setStrictMode(bool strict);
};

// Parse tree node types
class ParseTreeNode {
public:
    virtual ~ParseTreeNode() = default;
    virtual std::string getType() const = 0;
    virtual std::vector<std::shared_ptr<ParseTreeNode>> getChildren() const = 0;
};

}
```

### CHTLJSParser

Parser for CHTL JavaScript extensions.

```cpp
namespace chtl::parser::standalone {

class CHTLJSParser {
public:
    // Parse CHTL JS fragment
    std::unique_ptr<CHTLJSNode> parse(const std::string& fragment);
    
    // Check if fragment contains CHTL JS features
    bool hasCHTLJSFeatures(const std::string& fragment) const;
};

}
```

## Compiler API

### CompilerDispatcher

The main entry point for compiling CHTL source code.

```cpp
namespace chtl::compiler {

class CompilerDispatcher {
public:
    // Compile CHTL source to HTML/CSS/JS
    CompilationResult compile(const std::string& source);
    
    // Configuration
    void setMinifyOutput(bool minify);
    void setSourceMap(bool enable);
};

// Compilation result structure
struct CompilationResult {
    bool success;
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<CompilationError> errors;
    std::vector<CompilationWarning> warnings;
};

}
```

#### Usage Example

```cpp
#include <chtl/compiler/CompilerDispatcher.h>

using namespace chtl::compiler;

CompilerDispatcher dispatcher;
dispatcher.setMinifyOutput(true);

auto result = dispatcher.compile(chtlSource);
if (result.success) {
    // Write output files
    writeFile("output.html", result.html);
    writeFile("styles.css", result.css);
    writeFile("script.js", result.javascript);
} else {
    // Handle errors
    for (const auto& error : result.errors) {
        std::cerr << error.message << std::endl;
    }
}
```

## Code Generation API

### CHTLCodeGenVisitor

Visitor pattern implementation for generating code from parse trees.

```cpp
namespace chtl::parser::standalone {

class CHTLCodeGenVisitor {
public:
    // Visit parse tree and generate code
    void visit(const ParseTree& tree);
    
    // Get generated output
    GeneratedCode getGeneratedCode() const;
    
    // Configuration
    void setIndentSize(int spaces);
    void setPrettyPrint(bool enable);
};

// Generated code structure
struct GeneratedCode {
    std::string html;
    std::string css;
    std::string javascript;
    std::map<std::string, std::string> resources;
};

}
```

### Template Processing

```cpp
namespace chtl {

// Template manager for handling CHTL templates
class TemplateManager {
public:
    // Register template
    void registerTemplate(const std::string& name, 
                         std::shared_ptr<Template> templ);
    
    // Apply template
    std::string applyTemplate(const std::string& name,
                             const std::map<std::string, std::string>& params);
    
    // Check if template exists
    bool hasTemplate(const std::string& name) const;
};

}
```

## Error Handling API

### Error Management

```cpp
namespace chtl::error {

// Error levels
enum class ErrorLevel {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// Error categories
enum class ErrorCategory {
    SYNTAX,
    SEMANTIC,
    RUNTIME,
    SYSTEM
};

// Error information
struct ErrorInfo {
    ErrorLevel level;
    ErrorCategory category;
    std::string message;
    std::string file;
    size_t line;
    size_t column;
    std::string suggestion;
};

// Error manager
class ErrorManager {
public:
    // Report error
    void reportError(const ErrorInfo& error);
    
    // Get all errors
    std::vector<ErrorInfo> getErrors() const;
    
    // Clear errors
    void clearErrors();
    
    // Error statistics
    size_t getErrorCount(ErrorLevel level) const;
};

}
```

#### Error Handling Example

```cpp
#include <chtl/error/ErrorInterface.h>

using namespace chtl::error;

ErrorManager errorManager;

// Report an error
ErrorInfo error{
    ErrorLevel::ERROR,
    ErrorCategory::SYNTAX,
    "Unexpected token '}'",
    "main.chtl",
    15, 23,
    "Did you forget to open a block with '{'?"
};

errorManager.reportError(error);

// Check for errors
if (errorManager.getErrorCount(ErrorLevel::ERROR) > 0) {
    for (const auto& e : errorManager.getErrors()) {
        std::cerr << e.file << ":" << e.line << ":" << e.column 
                  << " " << e.message << std::endl;
    }
}
```

## Module System API

### Import Management

```cpp
namespace chtl {

// Import manager for handling module imports
class ImportManager {
public:
    // Process import statement
    void processImport(const std::string& importPath,
                      const std::string& alias = "");
    
    // Resolve module path
    std::string resolveModulePath(const std::string& moduleName) const;
    
    // Get imported symbols
    std::vector<std::string> getImportedSymbols() const;
    
    // Check circular dependencies
    bool hasCircularDependency() const;
};

// Module information
struct ModuleInfo {
    std::string name;
    std::string version;
    std::string path;
    std::vector<std::string> exports;
    std::vector<std::string> dependencies;
};

}
```

### CMOD Handling

```cpp
namespace chtl::module {

// CMOD package manager
class CMODManager {
public:
    // Pack directory into CMOD
    bool packModule(const std::string& directory,
                   const std::string& outputPath);
    
    // Unpack CMOD file
    bool unpackModule(const std::string& cmodPath,
                     const std::string& outputDirectory);
    
    // Get module metadata
    ModuleInfo getModuleInfo(const std::string& cmodPath) const;
    
    // Verify module integrity
    bool verifyModule(const std::string& cmodPath) const;
};

}
```

## Advanced Features

### CHTL JS Runtime API

```cpp
namespace chtl::runtime {

// CHTL JS runtime functions
class CHTLJSRuntime {
public:
    // Selector functions
    static std::string generateSelector(const std::string& selector);
    
    // Event handling
    static std::string generateEventListener(
        const std::string& element,
        const std::string& event,
        const std::string& handler
    );
    
    // Animation API
    static std::string generateAnimation(
        const std::string& element,
        const std::map<std::string, std::string>& properties,
        const AnimationOptions& options
    );
};

// Animation options
struct AnimationOptions {
    int duration;        // milliseconds
    std::string easing;  // easing function
    int delay;          // milliseconds
    bool loop;          // repeat animation
};

}
```

### Optimization API

```cpp
namespace chtl::optimization {

// CSS optimizer
class CSSOptimizer {
public:
    // Optimize CSS output
    std::string optimize(const std::string& css);
    
    // Configuration
    void setRemoveComments(bool remove);
    void setMergeSelectors(bool merge);
    void setMinifyColors(bool minify);
};

// JavaScript optimizer
class JSOptimizer {
public:
    // Optimize JavaScript output
    std::string optimize(const std::string& js);
    
    // Configuration
    void setMinify(bool enable);
    void setObfuscate(bool enable);
    void setTreeShaking(bool enable);
};

}
```

## Best Practices

### Memory Management

All API functions that return pointers use smart pointers (`std::unique_ptr` or `std::shared_ptr`) for automatic memory management.

### Thread Safety

- Scanner and parser classes are not thread-safe. Create separate instances for each thread.
- The module system uses internal locking for thread-safe access to shared resources.

### Error Handling

- All functions that can fail return boolean values or use exceptions
- Use the error management system to collect and report errors
- Always check return values and handle errors appropriately

### Performance Tips

1. Reuse parser and scanner instances when processing multiple files
2. Enable optimization flags for production builds
3. Use the caching system for frequently accessed modules
4. Consider parallel compilation for large projects

## Version Compatibility

This API reference is for CHTL version 1.0.0. Check the version using:

```cpp
#include <chtl/version.h>

std::cout << "CHTL Version: " << CHTL_VERSION_STRING << std::endl;
```