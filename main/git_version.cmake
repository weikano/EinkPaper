# 记录当前版本的git commit
# 1. 获取简短的 Git Commit Hash (例如: 7a3b1c2)
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 2. 获取 Git Commit Count (例如: 125)
execute_process(
    COMMAND git rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COUNT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 生成版本头文件
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/version.h.in"   # 源模板 (绝对路径)
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"    # 生成的目标文件
)
