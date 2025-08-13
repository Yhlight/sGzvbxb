#!/bin/bash

echo "=== 分析缺失的实现文件 ==="
echo

# 查找所有头文件
headers=$(find /workspace/src -name "*.h" -o -name "*.hpp" | grep -v "Example" | sort)

missing_count=0
implemented_count=0

echo "检查头文件对应的实现文件："
echo "================================"

for header in $headers; do
    # 获取基础名称（不包含扩展名）
    base_name=$(basename "$header" | sed -e 's/\.h$//' -e 's/\.hpp$//')
    dir_name=$(dirname "$header")
    
    # 查找对应的cpp文件
    cpp_file="${dir_name}/${base_name}.cpp"
    
    if [ -f "$cpp_file" ]; then
        implemented_count=$((implemented_count + 1))
        echo "✓ ${base_name}.h -> ${base_name}.cpp"
    else
        missing_count=$((missing_count + 1))
        echo "✗ ${base_name}.h -> 缺失实现文件"
        echo "  位置: ${header}"
    fi
done

echo
echo "================================"
echo "统计结果："
echo "已实现: $implemented_count"
echo "缺失: $missing_count"
echo "总计: $((implemented_count + missing_count))"