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

# setup adapters
set(ADAPTERS)

# org.freedesktop.PowerManagement.Inhibit
qt_add_dbus_adaptor(
    ADAPTERS
    share/org.freedesktop.PowerManagement.Inhibit.xml
    ${PROJECT_NAME}_powermanagement.h
    PowerManagement
    InhibitAdaptor
)

# org.freedesktop.ScreenSaver
qt_add_dbus_adaptor(
    ADAPTERS
    share/org.freedesktop.ScreenSaver.xml
    ${PROJECT_NAME}_screensaver.h
    PowerKit::ScreenSaver
    ScreenSaverAdaptor
)

# powerkit
set(SOURCES
    src/${PROJECT_NAME}.cpp
    src/${PROJECT_NAME}_app.cpp
    src/${PROJECT_NAME}_backlight.cpp
    src/${PROJECT_NAME}_client.cpp
    src/${PROJECT_NAME}_cpu.cpp
    src/${PROJECT_NAME}_device.cpp
    src/${PROJECT_NAME}_dialog.cpp
    src/${PROJECT_NAME}_manager.cpp
    src/${PROJECT_NAME}_notify.cpp
    src/${PROJECT_NAME}_powermanagement.cpp
    src/${PROJECT_NAME}_screensaver.cpp
    src/${PROJECT_NAME}_settings.cpp
    src/${PROJECT_NAME}_theme.cpp
)
set(HEADERS
    src/${PROJECT_NAME}_app.h
    src/${PROJECT_NAME}_backlight.h
    src/${PROJECT_NAME}_client.h
    src/${PROJECT_NAME}_common.h
    src/${PROJECT_NAME}_cpu.h
    src/${PROJECT_NAME}_device.h
    src/${PROJECT_NAME}_dialog.h
    src/${PROJECT_NAME}_manager.h
    src/${PROJECT_NAME}_notify.h
    src/${PROJECT_NAME}_powermanagement.h
    src/${PROJECT_NAME}_screensaver.h
    src/${PROJECT_NAME}_settings.h
    src/${PROJECT_NAME}_theme.h
)
add_executable(${PROJECT_NAME}
               ${SOURCES}
               ${HEADERS}
               ${ADAPTERS}
)
target_include_directories(${PROJECT_NAME}
                           PRIVATE
                           src
                           ${X11_X11_INCLUDE_PATH}
                           ${X11_Xrandr_INCLUDE_PATH}
                           ${X11_Xss_INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME}
                      ${X11_LIBRARIES}
                      ${X11_Xrandr_LIB}
                      ${X11_Xss_LIB}
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::DBus
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets)

# desktop files
configure_file(share/${PROJECT_NAME}.desktop
               ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop
               @ONLY)

# install
include(GNUInstallDirs)
install(TARGETS
        ${PROJECT_NAME}
        DESTINATION
        ${CMAKE_INSTALL_BINDIR})
install(FILES
        docs/${PROJECT_NAME}.1
        DESTINATION
        ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES
        LICENSE
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
