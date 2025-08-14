cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找必要的包
find_package(Threads REQUIRED)
find_package(Java QUIET COMPONENTS Runtime)

# ANTLR4运行时 - 使用已安装的版本

# Find ANTLR4 Runtime
set(ANTLR4_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/antlr4-runtime-install")
set(ANTLR4_INCLUDE_DIR "${ANTLR4_ROOT}/include/antlr4-runtime")
set(ANTLR4_LIB_DIR "${ANTLR4_ROOT}/lib")

# Find the static library (explicitly look for .a file)
find_library(ANTLR4_STATIC_LIB
    NAMES libantlr4-runtime.a antlr4-runtime
    PATHS ${ANTLR4_LIB_DIR}
    NO_DEFAULT_PATH
)

if(NOT ANTLR4_STATIC_LIB)
    message(FATAL_ERROR "ANTLR4 runtime library not found. Please install ANTLR4 runtime first.")
endif()

message(STATUS "Found ANTLR4 runtime: ${ANTLR4_STATIC_LIB}")
message(STATUS "ANTLR4 include dir: ${ANTLR4_INCLUDE_DIR}")

# 检查ANTLR工具（用于生成解析器）
find_program(ANTLR_EXECUTABLE
    NAMES antlr4 antlr
    PATHS /usr/local/bin /usr/bin
)

if(NOT ANTLR_EXECUTABLE)
    message(WARNING "ANTLR4 tool not found. Parser generation will be skipped.")
endif()

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/generated
    ${ANTLR4_INCLUDE_DIR}
)

# ===== CHTL核心 - 完全独立实现 =====
file(GLOB_RECURSE CHTL_CORE_SOURCES
    src/chtl/core/*.cpp
    src/chtl/generator/*.cpp
    src/chtl/template/*.cpp
    src/chtl/custom/*.cpp
    src/chtl/origin/*.cpp
    src/chtl/import/*.cpp
    src/chtl/namespace/*.cpp
    src/chtl/constraint/*.cpp
    src/chtl/module/*.cpp
    src/chtl/error/*.cpp
    src/chtl/utils/*.cpp
    src/chtl/CHTLContext.cpp  # 添加CHTLContext实现
    src/chtl/CHTLMainCompiler.cpp  # 添加主编译器
    src/chtl/CHTLTemplate.cpp  # 模板系统实现
    src/chtl/CHTLCustom.cpp  # 自定义系统实现
    src/chtl/CHTLCSSPreprocessor.cpp  # CSS预处理器（需要Template和Custom）
    src/chtl/CHTLGenerator.cpp  # 代码生成器实现
    # Temporarily commented out due to compilation errors
    src/chtl/CHTLConstraint.cpp  # 约束管理器实现 - 已修复
    # src/chtl/CHTLImport.cpp  # 导入管理器实现 - 需要ANTLR生成的CHTL解析器
    src/chtl/CHTLNamespace.cpp  # 命名空间管理器实现 - 已修复（使用适配器模式）
    src/chtl/CHTLScript.cpp  # 脚本管理器实现 - 已修复
    src/chtl/CHTLCMOD.cpp  # CMOD管理器实现 - 已修复
    src/chtl/CHTLOrigin.cpp  # Origin管理器实现 - 已修复
    src/chtl/CHTLImportVisitorStandalone.cpp  # 独立导入访问器实现
    src/chtl/ImportEnhanced.cpp  # 增强的导入系统实现
    src/chtl/AutoClassIdProcessor.cpp  # 自动化类/ID处理器
    src/chtl/stubs/ManagerStubs.cpp  # 临时管理器实现桩
)

# ===== 编译器架构 - 新增 =====
file(GLOB_RECURSE COMPILER_ARCH_SOURCES
    src/chtl/compiler/CompilerDispatcher.cpp
    src/chtl/compiler/handwritten/*.cpp
    # ANTLR编译器包装器移到css_js库
)

# 临时排除有问题的文件
list(REMOVE_ITEM SCANNER_SOURCES 
    ${CMAKE_SOURCE_DIR}/src/chtl/scanner/ScannerParserIntegration.cpp
    ${CMAKE_SOURCE_DIR}/src/chtl/scanner/ScannerIntegrator.cpp
)

# ===== CHTL独立解析器 =====
file(GLOB_RECURSE CHTL_PARSER_SOURCES
    src/chtl/parser/standalone/ParserRuntime.cpp
    src/chtl/parser/standalone/CHTLLexer.cpp
    src/chtl/parser/standalone/CHTLParser.cpp
    src/chtl/parser/standalone/CHTLParserEnhanced.cpp
    src/chtl/parser/standalone/CHTLCodeGenVisitor.cpp
    src/chtl/parser/standalone/CHTLJSLexer.cpp
    src/chtl/parser/standalone/CHTLJSParser.cpp
    src/chtl/parser/standalone/CHTLJSCodeGenerator.cpp
    src/chtl/parser/CHTLSimpleParser.cpp
)

# ===== 扫描器 - 独立实现 =====
file(GLOB_RECURSE SCANNER_SOURCES
    src/chtl/scanner/*.cpp
)

# 添加精准扫描器和状态机扫描器
set(SCANNER_SOURCES 
    ${SCANNER_SOURCES}
    src/chtl/scanner/PrecisionScanner.cpp
    src/chtl/scanner/StateMachineScanner.cpp
)

# ===== 新增模块 =====
# 缓存系统
file(GLOB_RECURSE CACHE_SOURCES
    src/chtl/cache/*.cpp
)

# 并行编译
file(GLOB_RECURSE PARALLEL_SOURCES
    src/chtl/parallel/*.cpp
)

# Source Map支持
file(GLOB_RECURSE SOURCEMAP_SOURCES
    src/chtl/sourcemap/*.cpp
)

# 模块系统
file(GLOB_RECURSE MODULE_SOURCES
    src/chtl/module/*.cpp
)

# 性能优化
file(GLOB_RECURSE OPTIMIZATION_SOURCES
    src/chtl/optimization/*.cpp
)

# 增强错误处理
set(ENHANCED_ERROR_SOURCES
    src/chtl/error/EnhancedError.cpp
)

# ===== 配置系统 - 由CHTL独立处理 =====
file(GLOB_RECURSE CONFIG_SOURCES
    # 使用不依赖ANTLR4的独立配置系统
    src/chtl/config/ConfigurationSystem.cpp
    src/chtl/parser/standalone/ConfigParser.cpp
)

# ===== CSS解析器 - 使用ANTLR4 =====
if(EXISTS "${CMAKE_SOURCE_DIR}/generated/css")
    file(GLOB_RECURSE CSS_ANTLR_SOURCES
        generated/css/*.cpp
    )
else()
    set(CSS_ANTLR_SOURCES "")
    message(WARNING "CSS parser not generated. Run setup_dependencies.sh first.")
endif()

file(GLOB CSS_COMPILER_SOURCES
    src/chtl/CHTLCSSCompiler.cpp
    # CHTLCSSPreprocessor.cpp moved to core
)

# ===== JS解析器 - 使用ANTLR4 =====
if(EXISTS "${CMAKE_SOURCE_DIR}/generated/javascript")
    file(GLOB_RECURSE JS_ANTLR_SOURCES
        generated/javascript/grammars/*.cpp
    )
else()
    set(JS_ANTLR_SOURCES "")
    message(WARNING "JS parser not generated. Run setup_dependencies.sh first.")
endif()

file(GLOB JS_COMPILER_SOURCES
    src/chtl/CHTLJSCompiler.cpp
    src/chtl/CHTLJSOptimizer.cpp
    src/chtl/optimizer/*.cpp
)

# ===== 主程序 =====
set(CHTL_MAIN_SOURCES
    src/main.cpp
)

# 创建库
# CHTL核心库（无ANTLR4依赖）
add_library(chtl_core STATIC
    ${CHTL_CORE_SOURCES}
    ${CHTL_PARSER_SOURCES}
    ${SCANNER_SOURCES}
    ${CONFIG_SOURCES}
    ${COMPILER_ARCH_SOURCES}
    ${CACHE_SOURCES}
    ${PARALLEL_SOURCES}
    ${SOURCEMAP_SOURCES}
    ${MODULE_SOURCES}
    ${OPTIMIZATION_SOURCES}
    ${ENHANCED_ERROR_SOURCES}
)

# CSS/JS库（依赖ANTLR4）
add_library(chtl_css_js STATIC
    ${CSS_ANTLR_SOURCES}
    ${CSS_COMPILER_SOURCES}
    ${JS_ANTLR_SOURCES}
    ${JS_COMPILER_SOURCES}
    src/chtl/compiler/antlr/CSSCompilerWrapper.cpp
    src/chtl/compiler/antlr/JSCompilerWrapper.cpp
)

# 添加对chtl_core的依赖以访问CHTLEnhancedCSSCompiler
add_dependencies(chtl_css_js chtl_core)

# 链接ANTLR4仅到CSS/JS库
target_link_libraries(chtl_css_js PUBLIC ${ANTLR4_STATIC_LIB})

# 主可执行文件
add_executable(chtl ${CHTL_MAIN_SOURCES})

# 链接（使用循环链接解决相互依赖）
target_link_libraries(chtl
    chtl_core
    chtl_css_js
    chtl_core  # 再次链接以解决循环依赖
    ${ANTLR4_STATIC_LIB}
    Threads::Threads
)

# 安装规则
install(TARGETS chtl
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/chtl
    DESTINATION include
)

# 模块
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/module
    DESTINATION share/chtl
)

# 示例
install(DIRECTORY ${CMAKE_SOURCE_DIR}/examples
    DESTINATION share/chtl
)

# 文档
install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs
    DESTINATION share/chtl
)

# 测试
enable_testing()

# 单元测试
file(GLOB_RECURSE TEST_SOURCES tests/unit/*.cpp)
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} 
        chtl_core 
        chtl_css_js 
        Threads::Threads
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# 自定义目标 - 生成解析器
if(ANTLR_EXECUTABLE AND Java_FOUND)
    add_custom_target(generate_parsers
        COMMAND ${CMAKE_SOURCE_DIR}/generate_parsers.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating ANTLR parsers for CSS and JavaScript"
    )
endif()

# 显示配置信息
message(STATUS "")
message(STATUS "CHTL Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "")
message(STATUS "Parser Implementation:")
message(STATUS "  CHTL Parser: Standalone (Native)")
message(STATUS "  CHTL JS Parser: Standalone (Native)")
message(STATUS "  Config Parser: Standalone (Native)")
message(STATUS "  Scanner: Standalone (Native)")
message(STATUS "  CSS Parser: ANTLR4")
message(STATUS "  JS Parser: ANTLR4")
message(STATUS "")
message(STATUS "Dependencies:")
message(STATUS "  ANTLR4 Runtime: ${antlr4-runtime_FOUND} (CSS/JS only)")
message(STATUS "  Threads: ${Threads_FOUND}")
message(STATUS "")