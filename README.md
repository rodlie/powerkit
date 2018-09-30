# powerdwarf

powerdwarf is a lightweight full featured power manager created for Slackware for use with alternative desktop environments and window managers, like Fluxbox, Blackbox, FVWM, WindowMaker, Openbox, Lumina and others.

## Features

 * Implements org.freedesktop.ScreenSaver daemon
   * Enables applications to inhibit the screen saver
   * Supports XScreenSaver
 * Implements org.freedesktop.PowerManagement daemon
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

 * In Fluxbox you need to add ``powerdwarf &`` to the ``~/.fluxbox/startup`` file
 * In Openbox add ``powerdwarf &`` to the ``~/.config/openbox/autostart`` file.

 **powerdwarf should not be used in GNOME or KDE.**

## Requirements

powerdwarf requires the following dependencies:

 * X11
 * Xss
 * Xrandr
 * Xinerama
 * QtDBus
 * QtXml
 * QtGui
 * QtCore
 * D-Bus
 * ConsoleKit or logind
 * UPower 0.9.23 (or higher, note that 0.99.x requires logind)
 * XScreenSaver
 * adwaita-icon-theme (or similar)

## Build

First make sure you have the required dependencies installed, then review the build options.

### Build options

 * **``PREFIX=</usr/local>``** : Install target.
 * **``XDGDIR=</etc/xdg>``** : Path to xdg autostart directory.
 * **``DOCDIR=<PREFIX/share/doc>``** : Path to the system documentation.
 * **``MANDIR=<PREFIX/share/man>``** : Path to the system manual.
 * **``UDEVDIR=</etc/udev>``** : Path to the udev directory.
 * **``CONFIG+=release``** : Creates a release build
 * **``CONFIG+=no_doc_install``** : Do not install application documentation.
 * **``CONFIG+=no_man_install``** : Do not install application manual.
 * **``CONFIG+=no_desktop_install``** : Do not install the application desktop file.
 * **``CONFIG+=no_autostart_install``** : Do not install the XDG autostart desktop file.
 * **``CONFIG+=install_udev_rules``** : Install additional power related udev (backlight) rules

### Build on Slackware

```
git clone https://github.com/rodlie/powerdwarf
cd powerdwarf
sudo GIT_COMMIT=`git rev-parse --short HEAD` ./powerdwarf.SlackBuild
sudo upgradepkg --install-new /tmp/powerdwarf-<VERSION>.tgz
```

### Build application

```
mkdir build && cd build
qmake CONFIG+=release .. && make
```
```
sudo make install
```

### Package application

```
qmake CONFIG+=release PREFIX=/usr
make
make INSTALL_ROOT=/pkg/path install
```
