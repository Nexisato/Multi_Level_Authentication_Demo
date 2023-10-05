cmake_minimum_required(VERSION 3.10)
project(auth_demo)

set(TARGET_NAME auth_test)
set(CMAKE_CXX_STANDARD 17)
set(OPENSSL_ROOT_DIR /usr/local/openssl)

find_package(OpenSSL REQUIRED)


include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

link_libraries(
    ${PROJECT_SOURCE_DIR}/lib    
)


set(OFFLINE_PART
    include/utils.h
    include/loader.h
    src/utils.cpp
    src/loader.cpp
    test/test_loader.cpp
)

set(ACCUMULATOR_PART
    include/utils.h
    include/accumulator.h
    src/utils.cpp
    src/accumulator.cpp
    test/test_accumulator.cpp
)

set(CLPKC_PART
    include/KGC.h
    include/Process.h
    src/KGC.cpp
    src/Process.cpp
    test/test_clpkc.cpp
)


# FILE(GLOB ${PROJECT_NAME}_SOURCES "src/*.cpp")
# FILE(GLOB ${PROJECT_NAME}_TEST "test/test_loader.cpp")

add_executable(${TARGET_NAME} 
    ${CLPKC_PART}
)

target_include_directories(${TARGET_NAME}  PRIVATE
    ${OPENSSL_ROOT_DIR}/include
)


target_link_libraries(${TARGET_NAME}  PRIVATE
    ${PROJECT_SOURCE_DIR}/lib/libpbc.so
    #${OPENSSL_ROOT_DIR}/lib64/libcrypto.so
    ${PROJECT_SOURCE_DIR}/lib/libjsoncpp.so
    libgmp.so
    libgmpxx.so
)
