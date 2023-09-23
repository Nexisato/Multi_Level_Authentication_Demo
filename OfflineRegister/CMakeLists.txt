cmake_minimum_required(VERSION 3.10.0)
project(offline_register)

set(TARGET_NAME offline_register)
set(CMAKE_CXX_STANDARD 17)

set(OPENSSL_ROOT_DIR /usr/local/openssl)

find_package(OpenSSL REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    /usr/local/include
    /usr/local/openssl/include
    )
link_directories(
    ${PROJECT_SOURCE_DIR}/lib
    /usr/local/lib
    /usr/local/openssl/lib
)

set(CODE_SOURCES 
    preprocess.cpp
)


add_executable(${TARGET_NAME} ${CODE_SOURCES})


target_include_directories( ${TARGET_NAME}  PRIVATE
    /usr/local/openssl/include
)


# 静态链接 PBC wrapper：这个是自己编译生成的需要手动添加
# gmp需要放到最后面
target_link_libraries(${TARGET_NAME} 
    ${OPENSSL_LIBRARIES}
)

