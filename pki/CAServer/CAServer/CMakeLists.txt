cmake_minimum_required(VERSION 3.5)

project(CAServer)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

set(SOURCES
    main.cpp
    mainwindow.cpp
    filerecv.cpp
    rootsign.cpp
    rvkcert.cpp
    rootverify.cpp
    jsonoper.cpp
)

set(HEADERS
    mainwindow.h
)

set(FORMS
    mainwindow.ui
)

set(RC_FILE
    certificate_server.rc
)

set(INCLUDE_DIRS
    /usr/local/include/openssl
)

set(LIBRARIES
    ssl
    crypto
)

add_executable(${PROJECT_NAME} 
${SOURCES} 
${HEADERS} 
${FORMS} 
${RC_FILE})

target_include_directories(${PROJECT_NAME} PRIVATE 
${INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE 
Qt5::Core 
Qt5::Gui
Qt5::Widgets 
Qt5::Network 
${LIBRARIES})