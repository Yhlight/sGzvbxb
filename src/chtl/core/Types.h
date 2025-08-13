#ifndef CHTL_CORE_TYPES_H
#define CHTL_CORE_TYPES_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {
namespace core {

// 基本的AST节点类型
enum class NodeType {
    ROOT,
    ELEMENT,
    TEXT,
    ATTRIBUTE,
    STYLE,
    SCRIPT,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    NAMESPACE,
    CONFIGURATION,
    COMMENT
};

// 代码片段类型
enum class FragmentType {
    CHTL,
    CSS,
    JAVASCRIPT,
    HTML,
    PLAIN_TEXT,
    UNKNOWN
};

// 基本的位置信息
struct SourceLocation {
    size_t line;
    size_t column;
    size_t offset;
    
    SourceLocation(size_t l = 0, size_t c = 0, size_t o = 0)
        : line(l), column(c), offset(o) {}
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    std::string content;
    SourceLocation start;
    SourceLocation end;
    
    CodeFragment(FragmentType t = FragmentType::UNKNOWN,
                 const std::string& c = "",
                 const SourceLocation& s = SourceLocation(),
                 const SourceLocation& e = SourceLocation())
        : type(t), content(c), start(s), end(e) {}
};

} // namespace core
} // namespace chtl

#endif // CHTL_CORE_TYPES_H