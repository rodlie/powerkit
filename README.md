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

## Compatibilty

On Fluxbox you need to add ``powerdwarf &`` to the ``~/.fluxbox/startup`` file. On Openbox add ``powerdwarf &`` to the ``~/.config/openbox/autostart`` file. If your desktop environment or window manager supports XDG auto start then powerdwarf should just work. powerdwarf should not be used in GNOME (and forks), KDE or Xfce.

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
 * ConsoleKit (optional, needed for power off)
 * UPower
 * XScreenSaver
 * adwaita-icon-theme (or similar)

## Build

First make sure you have the required dependencies installed, then review the build options.

### Build options

#### General

 * **``PREFIX=</usr/local>``** : Install target. Where files are copied when running ``make install``.
 * **``XDGDIR=</etc/xdg>``** : Path to xdg autostart directory, if ``PREFIX`` is ``/usr`` then ``XDGDIR`` should be ``/etc/xdg``. If ``PREFIX`` is ``/usr/local`` then ``XDGDIR`` should be ``/usr/local/etc/xdg``, it all depends on how your system is setup.
 * **``DOCDIR=<PREFIX/share/doc>``** : Path to the system documentation. Default is ``PREFIX/share/doc``.
 * **``UDEVDIR=</etc/udev>``** : Path to the udev directory, if ``PREFIX`` is ``/usr`` then ``UDEVDIR`` should be ``/etc/udev``. If ``PREFIX`` is ``/usr/local`` then ``UDEVDIR`` should be ``/usr/local/etc/udev``, it all depends on how your system is setup.
 * **``CONFIG+=release``** : Recommended option, this will create a release build without debug output.
 * **``CONFIG+=staticlib``** : Build static library. Recommended if you only want the application.

#### Optional application options

 * **``CONFIG+=no_app_install``** : Do not install the application.
 * **``CONFIG+=no_doc_install``** : Do not install application documentation.
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
```
usr
└── local
    ├── bin
    │   └── powerdwarf
    ├── etc
    │   └── xdg
    │       └── autostart
    │           └── powerdwarf.desktop
    └── share
        ├── applications
        │   └── powerdwarf.desktop
        └── doc
            └── powerdwarf-0.9.0
                ├── LICENSE
                └── README.md
```

### Package application

```
qmake CONFIG+=release CONFIG+=staticlib PREFIX=/usr
make
make INSTALL_ROOT=/pkg/path install
```
```              
pkg
├── etc
│   └── xdg
│       └── autostart
│           └── powerdwarf.desktop
└── usr
    ├── bin
    │   └── powerdwarf
    └── share
        ├── applications
        │   └── powerdwarf.desktop
        └── doc
            └── powerdwarf-0.9.0
                ├── LICENSE
                └── README.md

```

### Build library

```
mkdir build && cd build
qmake CONFIG+=release CONFIG+=install_lib CONFIG+=no_app_install .. && make
```
```
sudo make install
```
```
usr
└── local
    ├── include
    │   └── powerdwarf
    │       ├── common.h
    │       ├── hotplug.h
    │       ├── monitor.h
    │       ├── power.h
    │       ├── powermanagement.h
    │       ├── screensaver.h
    │       ├── service.h
    │       └── upower.h
    ├── lib
    │   ├── libPowerDwarf.so -> libPowerDwarf.so.0.9.0
    │   ├── libPowerDwarf.so.0 -> libPowerDwarf.so.0.9.0
    │   ├── libPowerDwarf.so.0.9 -> libPowerDwarf.so.0.9.0
    │   ├── libPowerDwarf.so.0.9.0
    │   └── pkgconfig
    │       └── PowerDwarf.pc
    └── share
        └── doc
            └── powerdwarf-0.9.0
                ├── LICENSE
                └── README.md
```

### Build and install everything

```
mkdir build && cd build
qmake CONFIG+=release CONFIG+=install_lib CONFIG+=install_udev_rules .. && make
```
```
sudo make install
```
```
usr
└── local
    ├── bin
    │   └── powerdwarf
    ├── etc
    │   ├── udev
    │   │   └── rules.d
    │   │       └── 90-backlight.rules
    │   └── xdg
    │       └── autostart
    │           └── powerdwarf.desktop
    ├── include
    │   └── powerdwarf
    │       ├── common.h
    │       ├── hotplug.h
    │       ├── monitor.h
    │       ├── power.h
    │       ├── powermanagement.h
    │       ├── screensaver.h
    │       ├── service.h
    │       └── upower.h
    ├── lib
    │   ├── libPowerDwarf.so -> libPowerDwarf.so.0.9.0
    │   ├── libPowerDwarf.so.0 -> libPowerDwarf.so.0.9.0
    │   ├── libPowerDwarf.so.0.9 -> libPowerDwarf.so.0.9.0
    │   ├── libPowerDwarf.so.0.9.0
    │   └── pkgconfig
    │       └── PowerDwarf.pc
    └── share
        ├── applications
        │   └── powerdwarf.desktop
        └── doc
            └── powerdwarf-0.9.0
                ├── LICENSE
                └── README.md
```
