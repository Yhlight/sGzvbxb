#pragma once

#include <memory>

namespace chtl {

// 前向声明
class CHTLStateMachine;

/**
 * CHTL JS 上下文 - 简化版本
 * TODO: 实现完整的上下文功能
 */
class CHTLJSContext {
public:
    CHTLJSContext() = default;
    ~CHTLJSContext() = default;
    
    // 获取状态机（暂时返回空指针）
    CHTLStateMachine* getStateMachine() { return nullptr; }
};

// CHTL JS 相关的别名
using JSContext = CHTLJSContext;

} // namespace chtl