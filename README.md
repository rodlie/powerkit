# PowerDwarf

PowerDwarf is a power manager created for Slackware for use with alternative desktop environments and window managers, like Fluxbox, FVWM, WindowMaker, Openbox, Lumina and others.

## Features

Some of the features included in PowerDwarf:

 * Implements org.freedesktop.ScreenSaver daemon specification
   * Enables applications to inhibit the screen saver
   * Supports XScreenSaver
 * Implements org.freedesktop.PowerManagement daemon specification
   * Enables applications to inhibit suspend actions
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

On Fluxbox you need to add ``powerdwarf &`` to the ``~/.fluxbox/startup`` file. On Openbox add ``powerdwarf &`` to the ``~/.config/openbox/autostart`` file. If your desktop environment or window manager supports XDG auto start then PowerDwarf should just work. PowerDwarf should not be used in GNOME (and forks), KDE or Xfce.

## Requirements

PowerDwarf requires the following dependencies to build:

 * Qt 4/5 core/gui/dbus/xml
 * XSS (X11 Screen Saver extension client library)
 * RandR
 * Xinerama

And the following during run-time:

 * ConsoleKit (optional, needed for power off)
 * UPower
 * xscreensaver
 * xrandr
 * adwaita-icon-theme (or similar)

## Build

PowerDwarf is relativity easy to build, just:

```
mkdir build && cd build
qmake CONFIG+=release CONFIG+=staticlib .. && make
sudo make install
```

For packaging use:

```
qmake CONFIG+=release CONFIG+=staticlib PREFIX=/usr
make
make INSTALL_ROOT=/pkg/path install
```

 * The XDG destination can be customized with ``XDGDIR=</etc/xdg>``
 * The documentation path can be customized with ``DOCDIR=<PREFIX/usr/share>``
 * Shared library is enabled by default, this can be disabled with ``CONFIG+=staticlib``
 * Library and headers are not installed by default, enable with ``CONFIG+=install_lib``
