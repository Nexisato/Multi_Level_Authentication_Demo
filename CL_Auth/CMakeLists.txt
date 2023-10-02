cmake_minimum_required(VERSION 3.10.0)
project(cl_demo)

set(TARGET_NAME cl_demo)
set(CMAKE_CXX_STANDARD 17)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    /usr/local/include
    )
link_directories(
    ${PROJECT_SOURCE_DIR}/lib
    /usr/local/lib
)

set(CODE_SOURCES 
    KGC.cpp
    Process.cpp
    test_clpkc.cpp
)


add_executable(${TARGET_NAME} ${CODE_SOURCES})


target_include_directories( ${TARGET_NAME}  PUBLIC 
    /usr/local/include
    ${PROJECT_SOURCE_DIR}/include
    )


# 静态链接 PBC wrapper：这个是自己编译生成的需要手动添加
# gmp需要放到最后面
target_link_libraries(${TARGET_NAME} 
    pbc
    gmp
)

