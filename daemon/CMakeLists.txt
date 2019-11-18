# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details

cmake_minimum_required(VERSION 3.0)
include(GNUInstallDirs)

project(powerkit VERSION 2.0.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

# set version
add_definitions(-DPOWERKIT_VERSION="${PROJECT_VERSION}")

# debug/release
add_compile_options(-Wall -Wextra)
if(CMAKE_BUILD_TYPE MATCHES "^(release|Release|RELEASE)$")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
else()
    add_definitions(-DQT_MESSAGELOGCONTEXT)
endif()

# qt
option(USE_QT5 "Use Qt 5" ON)
if(USE_QT5)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Network REQUIRED)
else()
    find_package(Qt4 REQUIRED QtCore QtNetwork)
endif()

# etc
set(DEFAULT_SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc")
if(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    set(DEFAULT_SYSCONFDIR "/etc")
endif()

# inc

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# exe
add_executable(${PROJECT_NAME}d main.cpp ${PROJECT_NAME}d.cpp)
add_executable(${PROJECT_NAME}-test test.cpp)

# link
if(USE_QT5)
    target_link_libraries(${PROJECT_NAME}d
                          Qt5::Core
                          Qt5::Network)
    target_link_libraries(${PROJECT_NAME}-test
                        Qt5::Core
                        Qt5::Network)
else()
    target_link_libraries(${PROJECT_NAME}d
                          Qt4::QtCore
                          Qt4::QtNetwork)
    target_link_libraries(${PROJECT_NAME}-test
                    Qt4::QtCore
                    Qt4::QtNetwork)
endif()

