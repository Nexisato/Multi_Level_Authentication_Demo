cmake_minimum_required(VERSION 3.10)
project(auth_demo)

set(TARGET_NAME auth_test)
set(CMAKE_CXX_STANDARD 17)

## 编译优化
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# # 添加 gprof 
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
# SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

find_package(OpenSSL REQUIRED)


# set(OPENSSL_INCLUDE_DIR /usr/include/openssl)
# set(OPENSSL_LIBRARIES /usr/lib64)




## Parallel Computing ##
# FIND_PACKAGE( OpenMP REQUIRED)
# if(OPENMP_FOUND)
#     message("OPENMP FOUND")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif()


include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

link_libraries(
    ${PROJECT_SOURCE_DIR}/lib    
)


set(OFFLINE_PART
    src/utils.cpp
    src/loader.cpp
    test/test_loader.cpp
)

set(ACCUMULATOR_PART
    src/utils.cpp
    src/accumulator.cpp
    test/test_accumulator.cpp
)

set(CLPKS_PART
    src/KGC_pbc.cpp
    src/Process_pbc.cpp
    src/utils.cpp
    test/test_clpks.cpp
)

set(SERIAL_TEST_PART
    src/utils.cpp
    src/KGC_pbc.cpp
    src/Process_pbc.cpp
    test/test_serial_unserial.cpp
)

set(PAIR_FREE_PART
    src/transfer.cpp
    src/KGC_pairFree.cpp
    src/Process_pairFree.cpp
    test/test_pairFree.cpp
)

# FILE(GLOB ${PROJECT_NAME}_SOURCES "src/*.cpp")
# FILE(GLOB ${PROJECT_NAME}_TEST "test/test_loader.cpp")

add_executable(${TARGET_NAME} 
    #${OFFLINE_PART}
    #${ACCUMULATOR_PART}
    ${CLPKS_PART}
    #${SERIAL_TEST_PART}
    #${PAIR_FREE_PART}
)

target_include_directories(${TARGET_NAME}  PRIVATE
    ${OPENSSL_ROOT_DIR}/include
)


target_link_libraries(${TARGET_NAME}  PRIVATE 
    libpbc.a
    libcrypto.so
    #OpenSSL::Crypto
    jsoncpp
    gmp
)
