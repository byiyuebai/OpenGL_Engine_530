cmake_minimum_required(VERSION 3.21)
project(MyProject)

# ========== 基础配置 ==========
# 设置 C++ 标准（强制生效）
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用编译器扩展（保证跨平台兼容性）

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# 禁用 CMake 内置的 GLFW 查找（避免与自定义 ku3f/lib 冲突）
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# ========== 头文件配置 ==========
# 搜索头文件
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
# 创建一个空列表用于存储目录
set(INCLUDE_DIRS "")
# 遍历所有头文件，提取目录并添加到列表中
foreach(header ${HEADERS})
    get_filename_component(dir ${header} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${dir})
endforeach()
# 对目录列表去重
list(REMOVE_DUPLICATES INCLUDE_DIRS)

# ========== 源文件配置 ==========
# 显式列出核心文件（CMake 能自动跟踪文件增减）
set(CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ku3f/glad.cpp
)

# 收集 src 下的其他文件（配合显式列出，兼顾便捷性）
file(GLOB_RECURSE SRC_OTHERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(APPEND CORE_SOURCES ${SRC_OTHERS})

# 头文件目录
set(INCLUDE_DIRS
    ${INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/ku3f/include
)

# 打印调试信息（可选，发布时可注释）
message(STATUS "=== 项目根目录: ${PROJECT_SOURCE_DIR}")
message(STATUS "=== 源文件列表: ${CORE_SOURCES}")
message(STATUS "=== 头文件目录: ${INCLUDE_DIRS}")

# ========== 目标配置 ==========
set(bin_name "out")


add_executable(${bin_name} 
               ${CORE_SOURCES})
 
# 头文件目录
target_include_directories(${bin_name} PRIVATE
    ${INCLUDE_DIRS}
)

# 链接库目录
target_link_directories(${bin_name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ku3f/lib
)

# 链接库文件
target_link_libraries(${bin_name} PRIVATE
    glfw3dll.lib
    assimp-vc145-mtd.lib
)

# ========== Windows 特定配置 ==========
if(WIN32)
    # 仅 MSVC 支持延迟加载（MinGW 等编译器不支持，需跳过）
    if(MSVC)
        # 设置 DLL 目录
        target_link_options(${bin_name} PRIVATE 
        "/DELAYLOAD:glfw3.dll"
        "/DELAYLOAD:assimp-vc145-mtd.dll"
        )
        # 链接延迟加载支持库
        target_link_libraries(${bin_name} PRIVATE delayimp.lib)
    endif()

    # 隐藏控制台窗口（可选：游戏通常不需要控制台，取消注释即可）
    set_target_properties(${bin_name} PROPERTIES
    #     WIN32_EXECUTABLE ON
         LINK_FLAGS "/ENTRY:mainCRTStartup"  # 确保入口点是 main（而非 WinMain）
    )
endif()

# ========== MSVC 编译器配置 ==========
if(MSVC)
    # 设置编码为 UTF-8（避免中文乱码）
    target_compile_options(${bin_name} PRIVATE /utf-8)
endif()