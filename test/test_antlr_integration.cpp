#include "../src/chtl/compiler/CompilerDispatcher.h"
#include <iostream>
#include <fstream>

using namespace chtl;

void testCSSParsing() {
    std::cout << "=== CSS Parsing Test ===" << std::endl;
    
    // 测试各种CSS语法
    std::string cssTests[] = {
        // 基本规则
        R"(
body {
    margin: 0;
    padding: 0;
    font-family: Arial, sans-serif;
}
)",
        // 嵌套选择器
        R"(
.container {
    width: 100%;
    max-width: 1200px;
    
    .header {
        background: #333;
        color: white;
        
        h1 {
            margin: 0;
            padding: 1rem;
        }
    }
}
)",
        // 媒体查询
        R"(
@media (max-width: 768px) {
    .container {
        width: 100%;
        padding: 0 15px;
    }
    
    .sidebar {
        display: none;
    }
}
)",
        // 动画
        R"(
@keyframes fadeIn {
    from {
        opacity: 0;
        transform: translateY(-10px);
    }
    to {
        opacity: 1;
        transform: translateY(0);
    }
}

.fade-element {
    animation: fadeIn 0.3s ease-out;
}
)",
        // CSS变量
        R"(
:root {
    --primary-color: #007bff;
    --secondary-color: #6c757d;
    --spacing-unit: 8px;
}

.button {
    background: var(--primary-color);
    padding: calc(var(--spacing-unit) * 2);
}
)"
    };
    
    compiler::CompilerDispatcher dispatcher;
    
    for (const auto& css : cssTests) {
        std::cout << "\nTesting CSS:\n" << css << std::endl;
        
        // 创建一个包含CSS的CHTL源码
        std::string source = "style {\n" + css + "\n}";
        
        auto result = dispatcher.compile(source);
        
        if (result.success) {
            std::cout << "✓ CSS parsed successfully" << std::endl;
            std::cout << "Generated CSS length: " << result.css.length() << " bytes" << std::endl;
        } else {
            std::cout << "✗ CSS parsing failed" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  Error: " << error.message << std::endl;
            }
        }
    }
}

void testJavaScriptParsing() {
    std::cout << "\n=== JavaScript Parsing Test ===" << std::endl;
    
    // 测试各种JavaScript语法
    std::string jsTests[] = {
        // ES6类
        R"(
class Component {
    constructor(name) {
        this.name = name;
        this.initialized = false;
    }
    
    init() {
        console.log(`Initializing ${this.name}`);
        this.initialized = true;
    }
    
    static create(name) {
        return new Component(name);
    }
}
)",
        // 箭头函数和模板字符串
        R"(
const greet = (name) => {
    return `Hello, ${name}!`;
};

const numbers = [1, 2, 3, 4, 5];
const doubled = numbers.map(n => n * 2);
const sum = numbers.reduce((acc, n) => acc + n, 0);
)",
        // 异步函数
        R"(
async function fetchData(url) {
    try {
        const response = await fetch(url);
        const data = await response.json();
        return data;
    } catch (error) {
        console.error('Failed to fetch data:', error);
        throw error;
    }
}

fetchData('/api/users')
    .then(users => console.log(users))
    .catch(err => console.error(err));
)",
        // 解构和扩展运算符
        R"(
const user = {
    name: 'John',
    age: 30,
    email: 'john@example.com'
};

const { name, ...rest } = user;
const newUser = { ...user, age: 31 };

const [first, second, ...others] = [1, 2, 3, 4, 5];
)",
        // 模块导入导出
        R"(
import React, { useState, useEffect } from 'react';
import * as utils from './utils';
import { API_URL } from './config';

export default function App() {
    const [data, setData] = useState(null);
    
    useEffect(() => {
        utils.fetchData(API_URL).then(setData);
    }, []);
    
    return data ? <div>{data}</div> : <Loading />;
}

export { utils, API_URL };
)"
    };
    
    compiler::CompilerDispatcher dispatcher;
    
    for (const auto& js : jsTests) {
        std::cout << "\nTesting JavaScript:\n" << js << std::endl;
        
        // 创建一个包含JavaScript的CHTL源码
        std::string source = "body { script {\n" + js + "\n} }";
        
        auto result = dispatcher.compile(source);
        
        if (result.success) {
            std::cout << "✓ JavaScript parsed successfully" << std::endl;
            std::cout << "Generated JS length: " << result.javascript.length() << " bytes" << std::endl;
        } else {
            std::cout << "✗ JavaScript parsing failed" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  Error: " << error.message << std::endl;
            }
        }
    }
}

void testErrorHandling() {
    std::cout << "\n=== Error Handling Test ===" << std::endl;
    
    struct ErrorTest {
        std::string name;
        std::string code;
        bool shouldFail;
    };
    
    ErrorTest tests[] = {
        {
            "Invalid CSS syntax",
            "style { body { color: } }",
            true
        },
        {
            "Invalid JavaScript syntax",
            "body { script { function( {} } }",
            true
        },
        {
            "Valid mixed content",
            R"(
style {
    body { margin: 0; }
}
body {
    h1 { text "Hello" }
    script {
        console.log('Initialized');
    }
}
)",
            false
        }
    };
    
    compiler::CompilerDispatcher dispatcher;
    
    for (const auto& test : tests) {
        std::cout << "\nTesting: " << test.name << std::endl;
        
        auto result = dispatcher.compile(test.code);
        
        bool failed = !result.success;
        if (failed == test.shouldFail) {
            std::cout << "✓ Test passed" << std::endl;
            if (failed) {
                std::cout << "  Expected errors:" << std::endl;
                for (const auto& error : result.errors) {
                    std::cout << "    - " << error.message << std::endl;
                }
            }
        } else {
            std::cout << "✗ Test failed" << std::endl;
            if (test.shouldFail) {
                std::cout << "  Expected to fail but succeeded" << std::endl;
            } else {
                std::cout << "  Expected to succeed but failed:" << std::endl;
                for (const auto& error : result.errors) {
                    std::cout << "    - " << error.message << std::endl;
                }
            }
        }
    }
}

int main() {
    std::cout << "CHTL ANTLR Parser Integration Test Suite\n" << std::endl;
    
    #ifdef USE_ANTLR_CSS
    std::cout << "ANTLR CSS Parser: ENABLED" << std::endl;
    #else
    std::cout << "ANTLR CSS Parser: DISABLED" << std::endl;
    #endif
    
    #ifdef USE_ANTLR_JS
    std::cout << "ANTLR JavaScript Parser: ENABLED" << std::endl;
    #else
    std::cout << "ANTLR JavaScript Parser: DISABLED" << std::endl;
    #endif
    
    std::cout << std::endl;
    
    testCSSParsing();
    testJavaScriptParsing();
    testErrorHandling();
    
    std::cout << "\nAll tests completed!" << std::endl;
    
    return 0;
}