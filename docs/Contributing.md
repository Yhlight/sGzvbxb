# Contributing to CHTL

Thank you for your interest in contributing to CHTL! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [Development Setup](#development-setup)
4. [Architecture Overview](#architecture-overview)
5. [Coding Standards](#coding-standards)
6. [Testing Guidelines](#testing-guidelines)
7. [Submitting Changes](#submitting-changes)
8. [Documentation](#documentation)
9. [Community](#community)

## Code of Conduct

We are committed to providing a welcoming and inspiring community for all. Please read and follow our [Code of Conduct](CODE_OF_CONDUCT.md).

- Be respectful and inclusive
- Welcome newcomers and help them get started
- Focus on constructive criticism
- Respect differing viewpoints and experiences

## Getting Started

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+
- Git
- Python 3.6+ (for some build scripts)
- ANTLR4 (optional, for CSS/JS parsers)

### First Steps

1. Fork the repository on GitHub
2. Clone your fork:
   ```bash
   git clone https://github.com/yourusername/chtl.git
   cd chtl
   ```
3. Add upstream remote:
   ```bash
   git remote add upstream https://github.com/chtl-project/chtl.git
   ```
4. Create a feature branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## Development Setup

### Building CHTL

```bash
# Create build directory
mkdir build && cd build

# Configure with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
make -j$(nproc)

# Run tests
make test
```

### Development Tools

We recommend using these tools:

- **IDE**: CLion, Visual Studio Code with C++ extensions
- **Code Formatter**: clang-format (config provided)
- **Static Analyzer**: clang-tidy, cppcheck
- **Debugger**: GDB or LLDB

### Setting up Development Environment

1. Install development dependencies:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake clang-format clang-tidy

   # macOS
   brew install cmake clang-format llvm
   ```

2. Install pre-commit hooks:
   ```bash
   ./scripts/install-hooks.sh
   ```

## Architecture Overview

Understanding CHTL's architecture is crucial for contributing effectively.

### Core Components

```
src/
├── chtl/
│   ├── scanner/           # Code scanning and fragmentation
│   │   ├── CHTLUnifiedScanner.*  # Main scanner
│   │   └── ...
│   ├── parser/            # Parsing components
│   │   ├── standalone/    # Native parsers
│   │   └── antlr/        # ANTLR-based parsers
│   ├── compiler/          # Compilation pipeline
│   │   ├── CompilerDispatcher.*
│   │   └── handwritten/   # Language-specific compilers
│   ├── error/            # Error handling system
│   ├── module/           # Module system
│   └── optimization/     # Code optimizers
```

### Key Design Principles

1. **Separation of Concerns**: Each component has a single, well-defined responsibility
2. **Modularity**: Components can be used independently
3. **Extensibility**: Easy to add new features without modifying core code
4. **Performance**: Compile-time optimization over runtime overhead

### Data Flow

```
Source Code → Scanner → Fragments → Compilers → Code Generation → Output
                ↓                        ↓
           Error Handler ←──────────────┘
```

## Coding Standards

### C++ Style Guide

We follow a modified Google C++ Style Guide with these specifics:

#### Naming Conventions

```cpp
// Classes and structs: PascalCase
class CompilerDispatcher;
struct CodeFragment;

// Functions and methods: camelCase
void processFragment();
bool hasErrors() const;

// Variables: camelCase
int fragmentCount;
std::string sourceCode;

// Constants: UPPER_SNAKE_CASE
const int MAX_FRAGMENT_SIZE = 1024;

// Namespaces: lowercase
namespace chtl {
namespace scanner {
```

#### Code Organization

```cpp
// Header file structure
#ifndef CHTL_COMPONENT_NAME_H
#define CHTL_COMPONENT_NAME_H

#include <system_headers>
#include "project_headers.h"

namespace chtl {

class ComponentName {
public:
    // Public methods
    
private:
    // Private methods and members
};

} // namespace chtl

#endif // CHTL_COMPONENT_NAME_H
```

#### Best Practices

1. **RAII**: Use smart pointers and RAII for resource management
2. **Const Correctness**: Mark methods and parameters `const` when appropriate
3. **Modern C++**: Prefer C++17 features (structured bindings, optional, etc.)
4. **Error Handling**: Use exceptions for exceptional cases, return types for expected errors

### Formatting

Use clang-format with the provided `.clang-format` file:

```bash
# Format a file
clang-format -i src/myfile.cpp

# Format all files
find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

## Testing Guidelines

### Test Structure

```
tests/
├── unit/           # Unit tests
├── integration/    # Integration tests
├── regression/     # Regression tests
└── benchmarks/     # Performance tests
```

### Writing Tests

#### Unit Tests (Google Test)

```cpp
#include <gtest/gtest.h>
#include "chtl/scanner/CHTLUnifiedScanner.h"

TEST(ScannerTest, HandlesEmptyInput) {
    chtl::scanner::CHTLUnifiedScanner scanner;
    auto fragments = scanner.scan("");
    EXPECT_TRUE(fragments.empty());
}

TEST(ScannerTest, ParsesSimpleHTML) {
    chtl::scanner::CHTLUnifiedScanner scanner;
    auto fragments = scanner.scan("body { text \"Hello\" }");
    ASSERT_EQ(fragments.size(), 1);
    EXPECT_EQ(fragments[0].type, chtl::scanner::FragmentType::CHTL);
}
```

#### Integration Tests

Create `.chtl` test files with expected outputs:

```chtl
// test_component.chtl
[Template] @Element TestCard {
    div {
        class: "card";
        h3 { text @title; }
    }
}

// test_component.expected.html
<div class="card">
    <h3>Test Title</h3>
</div>
```

### Running Tests

```bash
# Run all tests
make test

# Run specific test
./bin/test_unified_scanner

# Run with coverage
cmake -DCOVERAGE=ON ..
make coverage
```

### Test Coverage

Aim for:
- Unit tests: 80%+ coverage
- Critical paths: 100% coverage
- New features: Must include tests

## Submitting Changes

### Commit Messages

Follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
type(scope): subject

body

footer
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Test additions/changes
- `chore`: Build system/auxiliary tool changes

Examples:
```
feat(scanner): add support for CHTL JS context-aware parsing

Implemented context-aware parsing that detects CHTL JS continuations
like {{selector}}.method() and treats them as single fragments.

Closes #123
```

### Pull Request Process

1. **Update your branch**:
   ```bash
   git fetch upstream
   git rebase upstream/main
   ```

2. **Run tests locally**:
   ```bash
   make test
   ./scripts/run_tests.sh
   ```

3. **Create Pull Request**:
   - Use a clear, descriptive title
   - Reference related issues
   - Include a description of changes
   - Add screenshots for UI changes

4. **PR Template**:
   ```markdown
   ## Description
   Brief description of changes
   
   ## Type of Change
   - [ ] Bug fix
   - [ ] New feature
   - [ ] Breaking change
   - [ ] Documentation update
   
   ## Testing
   - [ ] Unit tests pass
   - [ ] Integration tests pass
   - [ ] Manual testing completed
   
   ## Checklist
   - [ ] Code follows style guidelines
   - [ ] Self-review completed
   - [ ] Documentation updated
   - [ ] Tests added/updated
   ```

### Code Review

- Be open to feedback
- Respond to all comments
- Make requested changes promptly
- Squash commits if requested

## Documentation

### Documenting Code

Use Doxygen-style comments:

```cpp
/**
 * @brief Scans CHTL source code and produces fragments
 * 
 * @param source The CHTL source code to scan
 * @return Vector of code fragments with type information
 * 
 * @throws CHTLException if source contains invalid syntax
 * 
 * @example
 * CHTLUnifiedScanner scanner;
 * auto fragments = scanner.scan("body { text \"Hello\" }");
 */
std::vector<CodeFragment> scan(const std::string& source);
```

### Updating Documentation

When adding features:
1. Update API documentation
2. Add examples to User Guide
3. Update relevant architecture docs
4. Add inline code comments

## Community

### Communication Channels

- **GitHub Issues**: Bug reports, feature requests
- **GitHub Discussions**: General discussions, Q&A
- **Discord**: Real-time chat, development coordination
- **Mailing List**: Major announcements, RFC discussions

### Getting Help

- Check existing issues and discussions
- Ask in Discord #help channel
- Review documentation and examples
- Contact maintainers for guidance

### Becoming a Maintainer

Active contributors may be invited to become maintainers. Criteria:
- Consistent high-quality contributions
- Good understanding of codebase
- Helpful to other contributors
- Commitment to project values

## Additional Resources

### Learning Resources

- [CHTL Grammar Documentation](../docs/CHTL_Grammar.md)
- [Architecture Overview](../docs/Architecture.md)
- [API Reference](../docs/API_Reference.md)

### Tools and Scripts

- `scripts/format-code.sh`: Format all code
- `scripts/run-tests.sh`: Run complete test suite
- `scripts/check-style.sh`: Check code style
- `scripts/update-deps.sh`: Update dependencies

### Debugging Tips

1. **Scanner Issues**:
   ```bash
   # Enable debug mode
   export CHTL_DEBUG=1
   ./bin/chtl_unified --debug input.chtl
   ```

2. **Parser Issues**:
   - Check parse tree output
   - Use AST visualization tools
   - Enable parser trace mode

3. **Memory Issues**:
   ```bash
   valgrind --leak-check=full ./bin/chtl_unified input.chtl
   ```

## Thank You!

Your contributions make CHTL better for everyone. We appreciate your time and effort in improving the project.

If you have questions or need help, don't hesitate to reach out!