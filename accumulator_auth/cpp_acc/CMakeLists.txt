cmake_minimum_required(VERSION 3.10.0)
project(RSA_ACC)

set(TARGET_NAME RSA_ACC)
set(CMAKE_CXX_STANDARD 17)

set(OPENSSL_ROOT_DIR /usr/local/openssl)

find_package(OpenSSL REQUIRED)

FIND_PACKAGE( OpenMP REQUIRED)

if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${OPENSSL_ROOT_DIR}$/include
    )
link_directories(
    ${PROJECT_SOURCE_DIR}/lib
    ${OPENSSL_ROOT_DIR}$/lib
)

set(CODE_SOURCES 
    rsa_acc.cpp
    utils.cpp
)


add_executable(${TARGET_NAME} ${CODE_SOURCES})


target_link_libraries(${TARGET_NAME} 
    ${OPENSSL_LIBRARIES}
    gmp
    gmpxx
    jsoncpp
)

