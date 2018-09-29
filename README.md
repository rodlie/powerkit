# powerdwarf

powerdwarf is a full featured power manager created for Slackware for use with alternative desktop environments and window managers, like Fluxbox, FVWM, WindowMaker, Openbox, Lumina and others.

## Features

Some of the features included in powerdwarf:

 * Implements org.freedesktop.ScreenSaver daemon specification
   * Enables applications to inhibit the screen saver
   * Supports XScreenSaver
 * Implements org.freedesktop.PowerManagement daemon specification
   * Enables applications to inhibit suspend actions
 * Lid action
 * Lock screen
 * Sleep
 * Hibernate
 * Shutdown
 * Inhibit suspend if external monitor connected
 * Automatically suspend based on timer/idle
 * Low and critical battery actions
 * Monitor hot-plug support (WIP)
 * Flexible configuration GUI

## Links

 * [Project](https://github.com/rodlie/powerdwarf)
 * [Tracker](https://github.com/rodlie/powerdwarf/issues)
 * [Milestones](https://github.com/rodlie/powerdwarf/milestones)
 * [Releases](https://github.com/rodlie/powerdwarf/releases)

## Usage

powerdwarf is a user session daemon and should be started during the user X11 startup session. If your desktop environment or window manager supports XDG auto start then powerdwarf should automatically start, if not you will need to add powerdwarf to your startup file (check the documentation included with your desktop environment or window manager).

 * On Fluxbox you need to add ``powerdwarf &`` to the ``~/.fluxbox/startup`` file
 * On Openbox add ``powerdwarf &`` to the ``~/.config/openbox/autostart`` file.

 **powerdwarf should not be used in GNOME or KDE.**

## Requirements

powerdwarf requires the following dependencies:

 * X11
 * Xss
 * Xrandr (lib+app)
 * Xinerama
 * QtDBus
 * QtXml
 * QtGui
 * QtCore
 * ConsoleKit or logind
 * UPower 0.9.23 (or higher, note that 0.99.x requires logind)
 * XScreenSaver
 * adwaita-icon-theme (or similar)
 
 The following distributions are supported/tested:
 * Slackware 14.2+
 * Ubuntu 18.04
 * FreeBSD 11.x

## Build

First make sure you have the required dependencies installed, then review the build options.

### Build options

#### General

 * **``PREFIX=</usr/local>``** : Install target. Where files are copied when running ``make install``.
 * **``XDGDIR=</etc/xdg>``** : Path to xdg autostart directory, if ``PREFIX`` is ``/usr`` then ``XDGDIR`` should be ``/etc/xdg``. If ``PREFIX`` is ``/usr/local`` then ``XDGDIR`` should be ``/usr/local/etc/xdg``, it all depends on how your system is setup.
 * **``DOCDIR=<PREFIX/share/doc>``** : Path to the system documentation. Default is ``PREFIX/share/doc``.
 * **``MANDIR=<PREFIX/share/man>``** : Path to the system manual. Default is ``PREFIX/share/man``.
 * **``UDEVDIR=</etc/udev>``** : Path to the udev directory, if ``PREFIX`` is ``/usr`` then ``UDEVDIR`` should be ``/etc/udev``. If ``PREFIX`` is ``/usr/local`` then ``UDEVDIR`` should be ``/usr/local/etc/udev``, it all depends on how your system is setup.
 * **``CONFIG+=release``** : Recommended option, this will create a release build without debug output.
 * **``CONFIG+=staticlib``** : Build static library. Recommended if you only want the application.

#### Optional application options

 * **``CONFIG+=no_app_install``** : Do not install the application.
 * **``CONFIG+=no_doc_install``** : Do not install application documentation.
 * **``CONFIG+=no_man_install``** : Do not install application manual.
 * **``CONFIG+=no_desktop_install``** : Do not install the application desktop file.
 * **``CONFIG+=no_autostart_install``** : Do not install the XDG autostart desktop file.
 * **``CONFIG+=install_udev_rules``** : Install additional power related udev rules

#### Optional library options

 * **``CONFIG+=install_lib``** : Install library.
 * **``CONFIG+=no_include_install``** : Do not install development files.
 * **``CONFIG+=no_pkgconfig_install``** : Do not install pkgconfig file.
 * **``CONFIG+=no_doc_install``** : Do not install library documentation.

### Build application

```
mkdir build && cd build
qmake CONFIG+=release CONFIG+=staticlib .. && make
```
```
sudo make install
```

### Package application

```
qmake CONFIG+=release CONFIG+=staticlib PREFIX=/usr
make
make INSTALL_ROOT=/pkg/path install
```

### Build library

```
mkdir build && cd build
qmake CONFIG+=release CONFIG+=install_lib CONFIG+=no_app_install .. && make
```

### Build everything

```
mkdir build && cd build
qmake CONFIG+=release CONFIG+=install_lib CONFIG+=install_udev_rules .. && make
```
