#!/bin/bash
# CHTL综合测试脚本

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# 输出函数
print_header() {
    echo -e "\n${GREEN}==== $1 ====${NC}\n"
}

print_error() {
    echo -e "${RED}[ERROR] $1${NC}"
}

print_success() {
    echo -e "${GREEN}[SUCCESS] $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}[WARNING] $1${NC}"
}

# 确保构建目录存在
if [ ! -d "$BUILD_DIR" ]; then
    print_error "Build directory not found. Please build the project first."
    exit 1
fi

cd "$BUILD_DIR"

# 运行单元测试
print_header "Running Unit Tests"
if make run_unit_tests; then
    print_success "Unit tests passed"
else
    print_error "Unit tests failed"
    exit 1
fi

# 运行现有测试
print_header "Running Component Tests"
if make run_all_tests; then
    print_success "Component tests passed"
else
    print_error "Component tests failed"
    exit 1
fi

# 运行编译测试
print_header "Running Compilation Tests"

test_files=(
    "test_simple.chtl"
    "test_scanner_precision.chtl"
    "test_unified_architecture.chtl"
)

for test_file in "${test_files[@]}"; do
    if [ -f "$PROJECT_ROOT/test/$test_file" ]; then
        echo "Testing $test_file..."
        output_file="$BUILD_DIR/output_${test_file%.chtl}.html"
        
        if timeout 10s "$BUILD_DIR/bin/chtl_unified" "$PROJECT_ROOT/test/$test_file" -o "$output_file" 2>&1; then
            if [ -f "$output_file" ]; then
                print_success "$test_file compiled successfully"
                # 检查输出文件大小
                size=$(stat -f%z "$output_file" 2>/dev/null || stat -c%s "$output_file" 2>/dev/null || echo "0")
                echo "  Output size: $size bytes"
            else
                print_error "$test_file: No output generated"
            fi
        else
            print_error "$test_file compilation failed or timed out"
        fi
    else
        print_warning "$test_file not found"
    fi
done

# 运行CTest
print_header "Running CTest Suite"
if command -v ctest &> /dev/null; then
    if ctest --output-on-failure; then
        print_success "CTest suite passed"
    else
        print_error "CTest suite failed"
        exit 1
    fi
else
    print_warning "CTest not found, skipping"
fi

# 生成测试报告
print_header "Test Summary"
echo "Test execution completed at: $(date)"

# 统计测试覆盖率（如果可用）
if command -v gcov &> /dev/null && [ -d "CMakeFiles" ]; then
    print_header "Coverage Report (if available)"
    # 这里可以添加覆盖率分析
    print_warning "Coverage analysis not yet implemented"
fi

print_success "All tests completed!"