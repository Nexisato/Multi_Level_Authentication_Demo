cmake_minimum_required(VERSION 3.0)

project(my_project)


set(TARGET_NAME my_executable)

find_package(OpenSSL REQUIRED)

include_directories(
    ${PROJECT_ROOT_DIR}
)


add_executable(${TARGET_NAME} 
    main.cpp
    KGC.hpp
    Process.hpp
    transfer.h
    )

target_link_libraries(${TARGET_NAME} 
    OpenSSL::Crypto
    gmp
)
