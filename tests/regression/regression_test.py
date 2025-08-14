#!/usr/bin/env python3
"""
CHTL回归测试框架
用于确保代码修改不会破坏现有功能
"""

import os
import sys
import json
import subprocess
import hashlib
import difflib
from datetime import datetime
from pathlib import Path

class RegressionTest:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        self.build_dir = self.project_root / "build"
        self.test_dir = self.project_root / "tests" / "regression"
        self.baseline_dir = self.test_dir / "baselines"
        self.compiler_path = self.build_dir / "bin" / "chtl_unified"
        
        # 确保目录存在
        self.baseline_dir.mkdir(parents=True, exist_ok=True)
        
        # 测试结果
        self.results = {
            "passed": [],
            "failed": [],
            "new": [],
            "timestamp": datetime.now().isoformat()
        }
    
    def run_all_tests(self):
        """运行所有回归测试"""
        print("=== CHTL Regression Testing ===\n")
        
        # 获取所有测试文件
        test_files = list(self.test_dir.glob("*.chtl"))
        
        if not test_files:
            print("No test files found in regression directory")
            return False
        
        # 运行每个测试
        for test_file in test_files:
            self.run_single_test(test_file)
        
        # 输出结果
        self.print_results()
        
        # 保存结果到文件
        self.save_results()
        
        return len(self.results["failed"]) == 0
    
    def run_single_test(self, test_file):
        """运行单个测试文件"""
        test_name = test_file.stem
        print(f"Testing {test_name}...")
        
        # 编译文件
        output_file = self.build_dir / f"regression_{test_name}.html"
        
        try:
            result = subprocess.run(
                [str(self.compiler_path), str(test_file), "-o", str(output_file)],
                capture_output=True,
                text=True,
                timeout=30
            )
            
            if result.returncode != 0:
                self.results["failed"].append({
                    "name": test_name,
                    "error": "Compilation failed",
                    "stderr": result.stderr
                })
                return
            
            # 读取输出
            with open(output_file, 'r') as f:
                output_content = f.read()
            
            # 计算输出的哈希值
            output_hash = hashlib.sha256(output_content.encode()).hexdigest()
            
            # 检查基准线
            baseline_file = self.baseline_dir / f"{test_name}.baseline"
            
            if baseline_file.exists():
                # 比较与基准线
                with open(baseline_file, 'r') as f:
                    baseline_data = json.load(f)
                
                if baseline_data["hash"] != output_hash:
                    # 输出有变化
                    diff = self.generate_diff(baseline_data["content"], output_content)
                    self.results["failed"].append({
                        "name": test_name,
                        "error": "Output differs from baseline",
                        "diff": diff
                    })
                else:
                    self.results["passed"].append(test_name)
            else:
                # 新测试，创建基准线
                baseline_data = {
                    "hash": output_hash,
                    "content": output_content,
                    "created": datetime.now().isoformat()
                }
                
                with open(baseline_file, 'w') as f:
                    json.dump(baseline_data, f, indent=2)
                
                self.results["new"].append(test_name)
                
        except subprocess.TimeoutExpired:
            self.results["failed"].append({
                "name": test_name,
                "error": "Compilation timeout"
            })
        except Exception as e:
            self.results["failed"].append({
                "name": test_name,
                "error": f"Unexpected error: {str(e)}"
            })
    
    def generate_diff(self, baseline, actual):
        """生成两个字符串的差异"""
        diff = difflib.unified_diff(
            baseline.splitlines(keepends=True),
            actual.splitlines(keepends=True),
            fromfile="baseline",
            tofile="actual"
        )
        return ''.join(diff)
    
    def print_results(self):
        """打印测试结果"""
        total = len(self.results["passed"]) + len(self.results["failed"]) + len(self.results["new"])
        
        print(f"\n=== Test Results ===")
        print(f"Total tests: {total}")
        print(f"Passed: {len(self.results['passed'])}")
        print(f"Failed: {len(self.results['failed'])}")
        print(f"New: {len(self.results['new'])}")
        
        if self.results["failed"]:
            print("\nFailed tests:")
            for test in self.results["failed"]:
                print(f"  - {test['name']}: {test['error']}")
                if "diff" in test and test["diff"]:
                    print("    Diff:")
                    for line in test["diff"].splitlines()[:10]:
                        print(f"    {line}")
                    if len(test["diff"].splitlines()) > 10:
                        print("    ... (truncated)")
        
        if self.results["new"]:
            print("\nNew tests (baselines created):")
            for test in self.results["new"]:
                print(f"  - {test}")
    
    def save_results(self):
        """保存测试结果到文件"""
        results_file = self.test_dir / "regression_results.json"
        with open(results_file, 'w') as f:
            json.dump(self.results, f, indent=2)
    
    def update_baseline(self, test_name):
        """更新特定测试的基准线"""
        test_file = self.test_dir / f"{test_name}.chtl"
        if not test_file.exists():
            print(f"Test file {test_name}.chtl not found")
            return False
        
        # 重新运行测试并更新基准线
        output_file = self.build_dir / f"regression_{test_name}.html"
        
        result = subprocess.run(
            [str(self.compiler_path), str(test_file), "-o", str(output_file)],
            capture_output=True,
            text=True
        )
        
        if result.returncode != 0:
            print(f"Failed to compile {test_name}")
            return False
        
        # 更新基准线
        with open(output_file, 'r') as f:
            output_content = f.read()
        
        output_hash = hashlib.sha256(output_content.encode()).hexdigest()
        
        baseline_data = {
            "hash": output_hash,
            "content": output_content,
            "updated": datetime.now().isoformat()
        }
        
        baseline_file = self.baseline_dir / f"{test_name}.baseline"
        with open(baseline_file, 'w') as f:
            json.dump(baseline_data, f, indent=2)
        
        print(f"Baseline updated for {test_name}")
        return True

def main():
    if len(sys.argv) < 2:
        print("Usage: regression_test.py <project_root> [update <test_name>]")
        sys.exit(1)
    
    project_root = sys.argv[1]
    tester = RegressionTest(project_root)
    
    if len(sys.argv) > 2 and sys.argv[2] == "update":
        if len(sys.argv) < 4:
            print("Please specify test name to update")
            sys.exit(1)
        success = tester.update_baseline(sys.argv[3])
        sys.exit(0 if success else 1)
    else:
        success = tester.run_all_tests()
        sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()