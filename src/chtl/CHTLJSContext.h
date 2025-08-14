#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include "CHTLStateMachine.h"
#include "CHTLJSCompiler.h"

namespace chtl {

// 前向声明
class ScriptBlock;

// JSSymbol 已在 CHTLJSCompiler.h 中定义
// 这里只添加需要的额外功能

/**
 * CHTL JS 上下文 - 完整版本
 * 管理 CHTL JS 的运行时环境和状态
 */
class CHTLJSContext {
public:
    CHTLJSContext() : stateMachine_(std::make_unique<CHTLStateMachine>()) {}
    ~CHTLJSContext() = default;
    
    // 状态机访问
    CHTLStateMachine& getStateMachine() { return *stateMachine_; }
    const CHTLStateMachine& getStateMachine() const { return *stateMachine_; }
    
    // 符号表管理
    void addSymbol(const JSSymbol& symbol) {
        symbolTable_[symbol.name] = symbol;
        // 判断是否是全局符号（根据当前作用域）
        if (scopeStack_.empty()) {
            globalSymbols_.push_back(symbol.name);
        }
    }
    
    JSSymbol* findSymbol(const std::string& name) {
        auto it = symbolTable_.find(name);
        return (it != symbolTable_.end()) ? &it->second : nullptr;
    }
    
    const std::vector<std::string>& getGlobalSymbols() const {
        return globalSymbols_;
    }
    
    // 作用域管理
    void enterScope(const std::string& scopeName) {
        scopeStack_.push_back(scopeName);
        currentScope_ = buildScopePath();
    }
    
    void exitScope() {
        if (!scopeStack_.empty()) {
            scopeStack_.pop_back();
            currentScope_ = buildScopePath();
        }
    }
    
    const std::string& getCurrentScope() const {
        return currentScope_;
    }
    
    // 错误处理
    void reportError(const std::string& error) {
        errors_.push_back(error);
        // 使用现有的状态机 API
        // stateMachine_->processEvent(CHTLEvent::ERROR_OCCURRED);
    }
    
    const std::vector<std::string>& getErrors() const {
        return errors_;
    }
    
    void clearErrors() {
        errors_.clear();
    }
    
    // 特性标志
    void enableFeature(const std::string& feature) {
        enabledFeatures_[feature] = true;
    }
    
    void disableFeature(const std::string& feature) {
        enabledFeatures_[feature] = false;
    }
    
    bool isFeatureEnabled(const std::string& feature) const {
        auto it = enabledFeatures_.find(feature);
        return (it != enabledFeatures_.end()) ? it->second : false;
    }
    
    // 回调注册
    using EventCallback = std::function<void(CHTLEvent)>;
    void registerEventCallback(CHTLEvent event, EventCallback callback) {
        eventCallbacks_[event] = callback;
    }
    
    void triggerEvent(CHTLEvent event) {
        stateMachine_->processEvent(event);
        auto it = eventCallbacks_.find(event);
        if (it != eventCallbacks_.end()) {
            it->second(event);
        }
    }
    
private:
    std::unique_ptr<CHTLStateMachine> stateMachine_;
    std::unordered_map<std::string, JSSymbol> symbolTable_;
    std::vector<std::string> globalSymbols_;
    std::vector<std::string> scopeStack_;
    std::string currentScope_;
    std::vector<std::string> errors_;
    std::unordered_map<std::string, bool> enabledFeatures_;
    std::unordered_map<CHTLEvent, EventCallback> eventCallbacks_;
    
    std::string buildScopePath() const {
        std::string path;
        for (const auto& scope : scopeStack_) {
            if (!path.empty()) path += ".";
            path += scope;
        }
        return path;
    }
};

// CHTL JS 相关的别名
using JSContext = CHTLJSContext;

} // namespace chtl