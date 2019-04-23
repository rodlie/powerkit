# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details

cmake_minimum_required(VERSION 3.0)

project(powerkit VERSION 1.9.0)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# set version
add_definitions(-DPOWERKIT_VERSION="${PROJECT_VERSION}")

# no debug output on release
if(CMAKE_BUILD_TYPE MATCHES RELEASE)
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

# qt
option(USE_QT5 "Use Qt 5" ON)
if(USE_QT5)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5DBus REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Widgets REQUIRED)
else()
    find_package(Qt4 REQUIRED QtCore QtDBus QtXml QtGui)
endif()

# x11
find_package(X11)

# etc
set(DEFAULT_SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc")
if(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    set(DEFAULT_SYSCONFDIR "/etc")
endif()

# docs
install(FILES
        docs/LICENSE
        docs/ChangeLog
        docs/README.md
        DESTINATION
        ${CMAKE_INSTALL_DOCDIR}-${PROJECT_VERSION})

# powerkitd
set(POWERKITD_SERVICE
    org.freedesktop.${PROJECT_NAME}d)
set(POWERKITD_USER
    "root"
    CACHE
    STRING
    "${PROJECT_NAME}d D-Bus user with access to /sys.")
set(POWERKITD_GROUP
    "users"
    CACHE
    STRING
    "group that have access to ${PROJECT_NAME}d D-Bus service, this should be any desktop user.")
set(POWERKITD_SOURCES
    src/${PROJECT_NAME}d.cpp
    src/${PROJECT_NAME}d_manager.cpp
    src/${PROJECT_NAME}_backlight.cpp
    src/${PROJECT_NAME}_cpu.cpp
    src/${PROJECT_NAME}_rtc.cpp)
set(POWERKITD_HEADERS
    src/${PROJECT_NAME}d_manager.h
    src/${PROJECT_NAME}_backlight.h
    src/${PROJECT_NAME}_cpu.h
    src/${PROJECT_NAME}_rtc.h)
add_executable(${PROJECT_NAME}d
               ${POWERKITD_SOURCES}
               ${POWERKITD_HEADERS})
target_include_directories(${PROJECT_NAME}d PUBLIC src)
if(USE_QT5)
    target_link_libraries(${PROJECT_NAME}d
                          Qt5::Core
                          Qt5::DBus)
else()
    target_link_libraries(${PROJECT_NAME}d
                          Qt4::QtCore
                          Qt4::QtDBus
                          Qt4::QtXml)
endif()
configure_file(share/${PROJECT_NAME}d.conf.in
               ${CMAKE_BINARY_DIR}/${POWERKITD_SERVICE}.conf
               @ONLY)
configure_file(share/${PROJECT_NAME}d.service.in
              ${CMAKE_BINARY_DIR}/${POWERKITD_SERVICE}.service
              @ONLY)
install(TARGETS
        ${PROJECT_NAME}d
        DESTINATION
        ${CMAKE_INSTALL_SBINDIR})
install(FILES
        ${CMAKE_BINARY_DIR}/${POWERKITD_SERVICE}.conf
        DESTINATION
        ${DEFAULT_SYSCONFDIR}/dbus-1/system.d)
install(FILES
        ${CMAKE_BINARY_DIR}/${POWERKITD_SERVICE}.service
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/dbus-1/system-services)

# powerkit-conf
set(POWERKIT_CONF_SOURCES
    src/${PROJECT_NAME}-conf.cpp
    src/${PROJECT_NAME}_settings.cpp
    src/${PROJECT_NAME}_dialog.cpp
    src/${PROJECT_NAME}_client.cpp
    src/${PROJECT_NAME}_theme.cpp
    src/${PROJECT_NAME}_backlight.cpp)
set(POWERKIT_CONF_HEADERS
    src/${PROJECT_NAME}_settings.cpp
    src/${PROJECT_NAME}_dialog.h
    src/${PROJECT_NAME}_client.h
    src/${PROJECT_NAME}_theme.h
    src/${PROJECT_NAME}_def.h
    src/${PROJECT_NAME}_backlight.h)
add_executable(${PROJECT_NAME}-conf
               ${POWERKIT_CONF_SOURCES}
               ${POWERKIT_CONF_HEADERS})
target_include_directories(${PROJECT_NAME}-conf PUBLIC src)
if(USE_QT5)
    target_link_libraries(${PROJECT_NAME}-conf
                          Qt5::Core
                          Qt5::DBus
                          Qt5::Gui
                          Qt5::Widgets)
else()
    target_link_libraries(${PROJECT_NAME}-conf
                          Qt4::QtCore
                          Qt4::QtDBus
                          Qt4::QtXml
                          Qt4::QtGui)
endif()
install(TARGETS
        ${PROJECT_NAME}-conf
        DESTINATION
        ${CMAKE_INSTALL_BINDIR})
install(FILES
        share/powerkit-conf.desktop
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/applications)
