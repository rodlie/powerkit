# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details

cmake_minimum_required(VERSION 3.14.3)

project(powerkit VERSION 2.0.0 LANGUAGES CXX)

set(PROJECT_HOMEPAGE_URL "https://github.com/rodlie/powerkit")
set(PROJECT_DESCRIPTION "Power manager for alternative X11 desktop environments and window managers")

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
    5.15
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
if(NOT X11_Xss_FOUND)
    message(FATAL_ERROR "Xss library not found")
endif()

# set proper etc path
set(DEFAULT_SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc")
if(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    set(DEFAULT_SYSCONFDIR "/etc")
endif()

# powerkitd user/group
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

# setup adapters
set(ADAPTERS)

# org.freedesktop.PowerManagement.Inhibit
qt_add_dbus_adaptor(
    ADAPTERS
    share/org.freedesktop.PowerManagement.Inhibit.xml
    powerkit_freedesktop_pm.h
    PowerManagement
    InhibitAdaptor
)

# org.freedesktop.ScreenSaver
qt_add_dbus_adaptor(
    ADAPTERS
    share/org.freedesktop.ScreenSaver.xml
    powerkit_screensaver.h
    PowerKit::ScreenSaver
    ScreenSaverAdaptor
)

# powerkitd
set(POWERKITD_SRC
    src/${PROJECT_NAME}d.cpp
    src/${PROJECT_NAME}d_manager.cpp
    src/${PROJECT_NAME}_backlight.cpp
    src/${PROJECT_NAME}_cpu.cpp
    src/${PROJECT_NAME}_rtc.cpp)
set(POWERKITD_INC
    src/${PROJECT_NAME}d_manager.h
    src/${PROJECT_NAME}_backlight.h
    src/${PROJECT_NAME}_cpu.h
    src/${PROJECT_NAME}_rtc.h)
add_executable(${PROJECT_NAME}d
               ${POWERKITD_SRC}
               ${POWERKITD_INC})

# powerkit
file(GLOB
     POWERKIT_SRC
     src/${PROJECT_NAME}.cpp
     "src/${PROJECT_NAME}_*.h"
     "src/${PROJECT_NAME}_*.cpp")
add_executable(${PROJECT_NAME}
               ${POWERKIT_SRC}
               ${ADAPTERS})

# includes and libraries
target_include_directories(${PROJECT_NAME}d PRIVATE src)
target_include_directories(${PROJECT_NAME}
                           PRIVATE
                           src
                           ${X11_X11_INCLUDE_PATH}
                           ${X11_Xrandr_INCLUDE_PATH}
                           ${X11_Xss_INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME}d
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::DBus)
target_link_libraries(${PROJECT_NAME}
                      ${X11_LIBRARIES}
                      ${X11_Xrandr_LIB}
                      ${X11_Xss_LIB}
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

# desktop files
configure_file(share/autostart.desktop.in
               ${CMAKE_BINARY_DIR}/autostart/${PROJECT_NAME}.desktop
               @ONLY)
configure_file(share/settings.desktop.in
               ${CMAKE_BINARY_DIR}/settings/${PROJECT_NAME}.desktop
               @ONLY)

# install
include(GNUInstallDirs)
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

# package
set(CPACK_SET_DESTDIR ON)
set(CPACK_PACKAGE_CONTACT ${PROJECT_HOMEPAGE_URL})
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR ${PROJECT_NAME})
set(CPACK_STRIP_FILES TRUE)

set(CPACK_RPM_SPEC_MORE_DEFINE "%define _build_id_links none")
set(CPACK_RPM_PACKAGE_LICENSE "BSD")
set(CPACK_RPM_PACKAGE_URL ${PROJECT_HOMEPAGE_URL})

set(CPACK_DEBIAN_PACKAGE_DEPENDS "xsecurelock")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${PROJECT_HOMEPAGE_URL})

include(CPack)
