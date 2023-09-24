cmake_minimum_required(VERSION 3.10.0)
project(RSA_ACC)

set(TARGET_NAME RSA_ACC)
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
    rsa_acc.cpp
    utils.cpp
)


add_executable(${TARGET_NAME} ${CODE_SOURCES})


target_link_libraries(${TARGET_NAME} 
    ${OPENSSL_LIBRARIES}
    ${PROJECT_SOURCE_DIR}/lib/libgmp.so
    ${PROJECT_SOURCE_DIR}/lib/libgmpxx.so
    jsoncpp
)

