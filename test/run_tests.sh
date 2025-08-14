#!/bin/bash

# CHTL 增强解析器测试套件

echo "==================================="
echo "CHTL Enhanced Parser Test Suite"
echo "==================================="

# 设置变量
CHTL_BIN="../build/bin/chtl"
TEST_DIR="."
OUTPUT_DIR="output"

# 创建输出目录
mkdir -p $OUTPUT_DIR

# 测试计数器
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# 运行测试的函数
run_test() {
    local test_file=$1
    local test_name=$2
    local output_file="$OUTPUT_DIR/${test_name}_output.html"
    
    echo -n "Testing $test_name... "
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # 运行编译器
    if $CHTL_BIN "$test_file" --enhanced -o "$output_file" > "$OUTPUT_DIR/${test_name}.log" 2>&1; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo "  Error log: $OUTPUT_DIR/${test_name}.log"
        return 1
    fi
}

# 基础测试
echo ""
echo "1. Basic Tests"
echo "--------------"
run_test "minimal.chtl" "minimal"
run_test "enhanced_no_comments.chtl" "no_comments"

# 注释测试
echo ""
echo "2. Comment Tests"
echo "----------------"
run_test "enhanced_comments.chtl" "comments"

# 高级特性测试
echo ""
echo "3. Advanced Feature Tests"
echo "-------------------------"
run_test "simple_template.chtl" "template"
run_test "enhanced_import.chtl" "import"

# 总结
echo ""
echo "==================================="
echo "Test Summary"
echo "==================================="
echo "Total tests: $TOTAL_TESTS"
echo "Passed: $PASSED_TESTS"
echo "Failed: $FAILED_TESTS"
echo ""

if [ $FAILED_TESTS -eq 0 ]; then
    echo "All tests passed! ✅"
    exit 0
else
    echo "Some tests failed. ❌"
    exit 1
fi