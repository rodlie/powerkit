# powerdwarf

powerdwarf is a power manager created for Slackware for use with alternative desktop environments and window managers, like Fluxbox, FVWM, WindowMaker, Openbox, Lumina and others.

## Features

Some of the features included in powerdwarf:

 * Implements org.freedesktop.ScreenSaver daemon specification
   * Enables applications to inhibit the screen saver (video player)
   * Supports XScreenSaver
 * Implements org.freedesktop.PowerManagement daemon specification
   * Enables applications to inhibit suspend actions (music player)
 * Supports lid and suspend actions
   * Lock screen
   * Sleep
   * Hibernate
   * Shutdown
   * Inhibit if external monitor connected
   * Automatically suspend based on timer/idle
 * Low and critical battery actions
 * Monitor hot-plug support
   * Save/Restore monitor states automatically
 * Flexible configuration GUI

## Links

 * [Project](https://github.com/rodlie/powerdwarf)
 * [Tracker](https://github.com/rodlie/powerdwarf/issues)
 * [Milestones](https://github.com/rodlie/powerdwarf/milestones)
 * [Releases](https://github.com/rodlie/powerdwarf/releases)
 
## Usage

powerdwarf is a daemon and should be started during the user X11 startup session. If your desktop environment or window manager supports XDG auto start then powerdwarf should automatically start, if not you will need to add powerdwarf to your startup file (check the documentation included with your desktop environment or window manager).

 * On Fluxbox you need to add ``powerdwarf &`` to the ``~/.fluxbox/startup`` file
 * On Openbox add ``powerdwarf &`` to the ``~/.config/openbox/autostart`` file.
 
 **powerdwarf should not be used in GNOME (and forks), KDE or Xfce.**

## Requirements

powerdwarf requires the following dependencies:

 * X11
 * Xss
 * Xrandr (lib)
 * Xinerama
 * QtDBus
 * QtXml
 * QtGui
 * QtCore
 * ConsoleKit (optional, needed for poweroff/restart)
 * UPower
 * XScreenSaver
 * adwaita-icon-theme (or similar)
 * xrandr (app)

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

### Build (and package) on Slackware

Installing powerdwarf on Slackware is easy, just:

```
git clone https://github.com/rodlie/powerdwarf
cd powerdwarf
sudo GIT_COMMIT=`git rev-parse --short HEAD` ./powerdwarf.SlackBuild
sudo installpkg /tmp/powerdwarf-VERSION.tgz
```

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
