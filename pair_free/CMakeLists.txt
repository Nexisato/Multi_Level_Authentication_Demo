cmake_minimum_required(VERSION 3.20)

project(my_project)


set(TARGET_NAME my_executable)

find_package(OpenSSL REQUIRED)

include_directories(
    ${PROJECT_ROOT_DIR}/include
)



add_executable(${TARGET_NAME} 
    src/transfer.cpp
    src/KGC_pairFree.cpp
    src/Process_pairFree.cpp
    test_pairFree.cpp
)

target_link_libraries(${TARGET_NAME} 
    OpenSSL::Crypto
    gmp
)
