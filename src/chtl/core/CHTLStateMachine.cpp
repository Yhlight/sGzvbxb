#include "../CHTLStateMachine.h"

namespace chtl {

CHTLStateMachine::CHTLStateMachine() : currentState(CHTLState::INITIAL) {
    // 初始化状态机
    initializeTransitionTable();
}

void CHTLStateMachine::initializeTransitionTable() {
    // 初始化状态转换表
    // 这里只添加一些基本的转换规则
    
    // 从INITIAL状态的转换
    transitionTable[CHTLState::INITIAL][CHTLEvent::IDENTIFIER] = CHTLState::IN_ELEMENT;
    transitionTable[CHTLState::INITIAL][CHTLEvent::OPEN_BRACKET] = CHTLState::IN_CONFIGURATION;
    // AT事件可能触发模板或自定义声明
    
    // 从IN_ELEMENT状态的转换
    transitionTable[CHTLState::IN_ELEMENT][CHTLEvent::OPEN_BRACE] = CHTLState::IN_ELEMENT;
    transitionTable[CHTLState::IN_ELEMENT][CHTLEvent::CLOSE_BRACE] = CHTLState::INITIAL;
    transitionTable[CHTLState::IN_ELEMENT][CHTLEvent::IDENTIFIER] = CHTLState::IN_ELEMENT;  // 嵌套元素
    // style/script通过IDENTIFIER事件处理
    
    // 从IN_CONFIGURATION状态的转换 ([Template], [Custom], etc.)
    transitionTable[CHTLState::IN_CONFIGURATION][CHTLEvent::CLOSE_BRACKET] = CHTLState::INITIAL;
    transitionTable[CHTLState::IN_CONFIGURATION][CHTLEvent::IDENTIFIER] = CHTLState::IN_CONFIGURATION;
    
    // 从IN_STYLE状态的转换
    transitionTable[CHTLState::IN_STYLE][CHTLEvent::OPEN_BRACE] = CHTLState::IN_ELEMENT;
    transitionTable[CHTLState::IN_STYLE][CHTLEvent::CLOSE_BRACE] = CHTLState::IN_ELEMENT;
    
    // 从IN_SCRIPT状态的转换
    transitionTable[CHTLState::IN_SCRIPT][CHTLEvent::OPEN_BRACE] = CHTLState::IN_ELEMENT;
    transitionTable[CHTLState::IN_SCRIPT][CHTLEvent::CLOSE_BRACE] = CHTLState::IN_ELEMENT;
}

bool CHTLStateMachine::processEvent(CHTLEvent event) {
    auto stateIt = transitionTable.find(currentState);
    if (stateIt != transitionTable.end()) {
        auto eventIt = stateIt->second.find(event);
        if (eventIt != stateIt->second.end()) {
            currentState = eventIt->second;
            return true;
        }
    }
    
    // 无效的状态转换
    errors.push_back("Invalid state transition");
    return false;
}

void CHTLStateMachine::pushState(CHTLState state) {
    stateStack.push(currentState);
    currentState = state;
}

bool CHTLStateMachine::popState() {
    if (!stateStack.empty()) {
        currentState = stateStack.top();
        stateStack.pop();
        return true;
    }
    return false;
}

bool CHTLStateMachine::isValidTransition(CHTLState from, CHTLEvent event, CHTLState to) {
    auto stateIt = transitionTable.find(from);
    if (stateIt != transitionTable.end()) {
        auto eventIt = stateIt->second.find(event);
        if (eventIt != stateIt->second.end()) {
            return eventIt->second == to;
        }
    }
    return false;
}

void CHTLStateMachine::reset() {
    // 清空状态栈
    while (!stateStack.empty()) {
        stateStack.pop();
    }
    currentState = CHTLState::INITIAL;
    errors.clear();
}

} // namespace chtl