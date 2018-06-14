# PowerDwarf

PowerDwarf is a power manager created for Slackware for use with alternative desktop environments and window managers, like Fluxbox, FVWM, WindowMaker, Openbox, Lumina and others.

## Features

Some of the features included in PowerDwarf:

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
 * Small (under 400k)

## Compatibilty

On Fluxbox you need to add ``powerdwarf &`` to the ``~/.fluxbox/startup`` file. On Openbox add ``powerdwarf &`` to the ``~/.config/openbox/autostart`` file. If your desktop environment or window manager supports XDG auto start then PowerDwarf should just work. PowerDwarf should not be used in GNOME (and forks), KDE or Xfce.

## Requirements

PowerDwarf requires the following dependencies:

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

PowerDwarf is relativity easy to build, just:

```
mkdir build && cd build
qmake CONFIG+=release .. && make
sudo make install
```

For packaging use:

```
qmake CONFIG+=release PREFIX=/usr
make
make INSTALL_ROOT=/pkg/path install
```
```
pkg/
├── etc
│   └── xdg
│       └── autostart
│           └── powerdwarf.desktop
└── usr
    ├── bin
    │   ├── powerdwarf
    │   └── powerdwarf-config
    ├── include
    │   └── powerdwarf
    │       ├── common.h
    │       ├── hotplug.h
    │       ├── monitor.h
    │       ├── power.h
    │       ├── powermanagement.h
    │       ├── screensaver.h
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

 * The XDG destination can be customized with ``XDGDIR=</etc/xdg>``
 * The documentation path can be customized with ``DOCDIR=<PREFIX/usr/share>``
 * Shared library is enabled by default, this can be disabled with ``CONFIG+=staticlib``
 * Library installation can be disabled with ``echo "CONFIG-=install_lib" > local.pri``
