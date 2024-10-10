# 设置 cmake 的最小需求
cmake_minimum_required(VERSION 3.10)

# 生成 clangd 配置文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 创建项目并设置项目版本
project(LinuxCpp VERSION 1.0)

# 根据谷歌 cpp 规范, 新的 cpp 项目应该至少面向 cpp20 构建
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 创建一个动态链接库
add_library(SayHelloShared SHARED src/shared/hello_shared.cc)
target_include_directories(SayHelloShared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 创建一个静态链接库
add_library(SayHelloStatic STATIC src/static/hello_static.cc)
target_include_directories(SayHelloStatic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 创建一个可执行文件
add_executable(SayHello src/main.cc)
target_include_directories(SayHello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# 将动态链接库和静态链接库链接到可执行文件中
target_link_libraries(SayHello SayHelloShared SayHelloStatic)