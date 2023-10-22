cmake_minimum_required(VERSION 3.5)

project(AP LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENSSL_ROOT_DIR /usr/local/openssl)
set(OPENSSL_LIBRARIES /usr/local/openssl/lib64)
set(OPENSSL_INCLUDE_DIR /usr/local/openssl/include)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

link_libraries(
    ${PROJECT_SOURCE_DIR}/lib
)

if(ANDROID)
  add_library(AP SHARED
    main.cpp
    login.cpp
    login.h
    login.ui
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    filechosewindow.cpp
    filechosewindow.h
    filechosewindow.ui
    homewindow.cpp
    homewindow.h
    homewindow.ui
    loader.h
    loader.cpp
    utils.cpp
  )
else()
  add_executable(AP
    main.cpp
    login.cpp
    login.h
    login.ui
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    filechosewindow.cpp
    filechosewindow.h
    filechosewindow.ui
    homewindow.cpp
    homewindow.h
    homewindow.ui
    include/loader.h
    include/utils.h
    loader.cpp
    utils.cpp
  )
endif()

target_link_libraries(AP PRIVATE
    Qt5::Widgets
    ${OPENSSL_ROOT_DIR}/lib64/libcrypto.so
    libjsoncpp.so
    libgmp.so
    libgmpxx.so
    )
