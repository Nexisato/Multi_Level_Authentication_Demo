cmake_minimum_required(VERSION 3.10)
project(auth_demo)

set(TARGET_NAME auth_test)
set(CMAKE_CXX_STANDARD 17)


# 添加 gprof 
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")


set(OPENSSL_ROOT_DIR /usr/local/openssl)
set(OPENSSL_LIBRARIES /usr/local/openssl/lib64)
set(OPENSSL_INCLUDE_DIR /usr/local/openssl/include)

#find_package(OpenSSL REQUIRED)

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
    src/KGC.cpp
    src/Process.cpp
    src/utils.cpp
    test/test_clpks.cpp
)

set(CLPKS_WRAPPER_PART
    src/KGC_wrapper.cpp
    src/Process_wrapper.cpp
    src/utils.cpp
    test/test_clpks_wrapper.cpp
)

# FILE(GLOB ${PROJECT_NAME}_SOURCES "src/*.cpp")
# FILE(GLOB ${PROJECT_NAME}_TEST "test/test_loader.cpp")

add_executable(${TARGET_NAME} 
    #${OFFLINE_PART}
    #${ACCUMULATOR_PART}
    #####${CLPKS_PART}
    ${CLPKS_WRAPPER_PART}
)

target_include_directories(${TARGET_NAME}  PRIVATE
    ${OPENSSL_ROOT_DIR}/include
)


target_link_libraries(${TARGET_NAME}  PRIVATE
    ${PROJECT_SOURCE_DIR}/lib/libPBC.a    
    ${PROJECT_SOURCE_DIR}/lib/libpbc.a
    ${OPENSSL_LIBRARIES}/libcrypto.so
    libjsoncpp.so
    libgmp.so
    libgmpxx.so
)
