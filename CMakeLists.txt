# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

include("${CMAKE_CURRENT_LIST_DIR}/main/git_version.cmake")

project(lvgl_m5)

# elf符号表目录
set(ELF_SYMBOLS_DIR "${PROJECT_SOURCE_DIR}/mappings/symbos")
# 固件bin目录
set(IMAGE_DIR "${PROJECT_SOURCE_DIR}/mappings/bins")
# map目录
set(MAP_DIR "${PROJECT_SOURCE_DIR}/mappings/maps")
# 创建目录
file(MAKE_DIRECTORY "${ELF_SYMBOLS_DIR}")
file(MAKE_DIRECTORY "${IMAGE_DIR}")
file(MAKE_DIRECTORY "${MAP_DIR}")

set(OUTPUT_TARGET_NAME "${GIT_COMMIT_TIME}_${GIT_HASH}")

# message("OUTPUT_TARGET_NAME: ${OUTPUT_TARGET_NAME}")
# message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
# message("CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")
# message("ELF_FILE: ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf")
# message("BIN_FILE: ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin")
# message("ELF_SYMBOLS_DIR: ${ELF_SYMBOLS_DIR}")
# message("IMAGE_DIR: ${IMAGE_DIR}")
# message("MAP_DIR: ${MAP_DIR}")


# 复制elf符号表
# ${CMAKE_PROJECT_NAME}.elf 是 ESP-IDF 默认生成的符号表文件名
add_custom_command(
    TARGET app POST_BUILD
    # 复制elf文件
    COMMAND ${CMAKE_COMMAND} -E copy 
            "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf" 
            "${ELF_SYMBOLS_DIR}/${OUTPUT_TARGET_NAME}.elf"
    # 复制 BIN (固件)
    COMMAND ${CMAKE_COMMAND} -E copy 
            "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin" 
            "${IMAGE_DIR}/${OUTPUT_TARGET_NAME}.bin"
    # 复制 MAP 文件
    COMMAND ${CMAKE_COMMAND} -E copy 
            "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map" 
            "${MAP_DIR}/${OUTPUT_TARGET_NAME}.map"
    COMMAND ${CMAKE_COMMAND} -E echo "Image Archived to: ${IMAGE_DIR}/${OUTPUT_TARGET_NAME}.bin"
    COMMAND ${CMAKE_COMMAND} -E echo "Symbol file archived to: ${ELF_SYMBOLS_DIR}/${OUTPUT_TARGET_NAME}.elf"
    COMMAND ${CMAKE_COMMAND} -E echo "Map file archived to: ${MAP_DIR}/${OUTPUT_TARGET_NAME}.map"
    COMMENT "Archiving symbols, binary, and map files with Git Hash"
)                    
