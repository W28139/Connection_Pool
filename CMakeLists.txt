cmake_minimum_required(VERSION 3.10)

# 项目名称
project(ConnectionPool)

# 1. 设置 C++ 标准 (连接池需要 lambda 和 智能指针)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. 设置可执行文件的输出目录到 bin 文件夹
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 3. 指定头文件搜索路径 (指向你的 include 目录)
include_directories(${CMAKE_SOURCE_DIR}/include)

# 4. 自动搜集 src 目录下的所有源文件 (.cpp)
# 这样以后你在 src 下增加文件，不需要手动修改这个文件
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# 5. 生成可执行程序
add_executable(connection_pool ${SRC_FILES})

# 6. 链接库文件
# mysqlclient: MySQL 的 C 客户端库
# pthread: Linux 多线程库 (连接池必需)
target_link_libraries(connection_pool mysqlclient pthread)

# 打印信息用于确认
message(STATUS "Found source files: ${SRC_FILES}")
message(STATUS "Executable will be generated in: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")