# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details

cmake_minimum_required(VERSION 3.0)
include(GNUInstallDirs)

project(powerkit VERSION 2.0.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
find_package(QT NAMES Qt5 COMPONENTS Core REQUIRED)
find_package(
    Qt${QT_VERSION_MAJOR}
    5.6
    COMPONENTS
    Gui
    Widgets
    DBus
    REQUIRED
)

# x11
find_package(X11 REQUIRED)
if(NOT X11_Xrandr_FOUND)
    message(FATAL_ERROR "Xrandr library not found")
endif()
if(${CMAKE_VERSION} VERSION_LESS "3.14.3")
    if(NOT X11_Xscreensaver_FOUND)
        message(FATAL_ERROR "Xscreensaver library not found")
    endif()
else()
    if(NOT X11_Xss_FOUND)
        message(FATAL_ERROR "Xss library not found")
    endif()
endif()

# etc
set(DEFAULT_SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc")
if(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    set(DEFAULT_SYSCONFDIR "/etc")
endif()

# service
set(SERVICE_NAME
    org.freedesktop.PowerKit)
set(SERVICE_USER
    "root"
    CACHE
    STRING
    "D-Bus service user with access to /sys.")
set(SERVICE_GROUP
    "users"
    CACHE
    STRING
    "group that have access to D-Bus service, this should be any desktop user.")

# org.freedesktop.PowerManagement.Inhibit
set(ADAPTERS)
qt_add_dbus_adaptor(
    ADAPTERS
    share/org.freedesktop.PowerManagement.Inhibit.xml
    powerkit_freedesktop_pm.h
    PowerManagement
    InhibitAdaptor
)

# exe
set(SOURCES
    src/${PROJECT_NAME}d.cpp
    src/${PROJECT_NAME}d_manager.cpp
    src/${PROJECT_NAME}_backlight.cpp
    src/${PROJECT_NAME}_cpu.cpp
    src/${PROJECT_NAME}_rtc.cpp)
set(HEADERS
    src/${PROJECT_NAME}d_manager.h
    src/${PROJECT_NAME}_backlight.h
    src/${PROJECT_NAME}_cpu.h
    src/${PROJECT_NAME}_rtc.h)
add_executable(${PROJECT_NAME}d
               ${SOURCES}
               ${HEADERS})
file(GLOB
     POWERKIT_SRC
     "src/main.cpp"
     "src/${PROJECT_NAME}.h"
     "src/${PROJECT_NAME}.cpp"
     "src/${PROJECT_NAME}_*.h"
     "src/${PROJECT_NAME}_*.cpp")
add_executable(${PROJECT_NAME}
               ${POWERKIT_SRC}
               ${ADAPTERS})

# link
if(${CMAKE_VERSION} VERSION_LESS "3.14.3")
    set(XSS_INCLUDE ${X11_Xscreensaver_INCLUDE_PATH})
    set(XSS_LIB ${X11_Xscreensaver_LIB})
else()
    set(XSS_INCLUDE ${X11_Xss_INCLUDE_PATH})
    set(XSS_LIB ${X11_Xss_LIB})
endif()
target_include_directories(${PROJECT_NAME}d PUBLIC src)
target_include_directories(${PROJECT_NAME}
                           PUBLIC
                           src
                           ${X11_X11_INCLUDE_PATH}
                           ${X11_Xrandr_INCLUDE_PATH}
                           ${XSS_INCLUDE})
target_link_libraries(${PROJECT_NAME}
                      ${X11_LIBRARIES}
                      ${X11_Xrandr_LIB}
                      ${XSS_LIB})
target_link_libraries(${PROJECT_NAME}d
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::DBus)
target_link_libraries(${PROJECT_NAME}
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::DBus
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets)


# conf
configure_file(share/conf.in
               ${CMAKE_BINARY_DIR}/${SERVICE_NAME}.conf
               @ONLY)
configure_file(share/service.in
               ${CMAKE_BINARY_DIR}/${SERVICE_NAME}.service
               @ONLY)

# desktop
configure_file(share/autostart.desktop.in
               ${CMAKE_BINARY_DIR}/autostart/${PROJECT_NAME}.desktop
               @ONLY)
configure_file(share/settings.desktop.in
               ${CMAKE_BINARY_DIR}/settings/${PROJECT_NAME}.desktop
               @ONLY)

# install
install(TARGETS
        ${PROJECT_NAME}d
        DESTINATION
        ${CMAKE_INSTALL_LIBEXECDIR})
install(TARGETS
        ${PROJECT_NAME}
        DESTINATION
        ${CMAKE_INSTALL_BINDIR})
install(FILES
        ${CMAKE_BINARY_DIR}/${SERVICE_NAME}.conf
        DESTINATION
        ${DEFAULT_SYSCONFDIR}/dbus-1/system.d)
install(FILES
        ${CMAKE_BINARY_DIR}/${SERVICE_NAME}.service
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/dbus-1/system-services)
install(FILES
        share/${PROJECT_NAME}.1
        DESTINATION
        ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES
        share/${PROJECT_NAME}d.8
        DESTINATION
        ${CMAKE_INSTALL_MANDIR}/man8)
install(FILES
        ${CMAKE_BINARY_DIR}/settings/${PROJECT_NAME}.desktop
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES
        ${CMAKE_BINARY_DIR}/autostart/${PROJECT_NAME}.desktop
        DESTINATION
        ${DEFAULT_SYSCONFDIR}/xdg/autostart)
install(FILES
        LICENSE
        docs/README.md
        DESTINATION
        ${CMAKE_INSTALL_DOCDIR}/${PROJECT_NAME}-${PROJECT_VERSION})
