# powerdwarf

powerdwarf is a lightweight full featured power manager created for Slackware for use with alternative desktop environments and window managers, like Fluxbox, Blackbox, FVWM, WindowMaker, Openbox, Lumina and others.

## Features

 * Enables applications to inhibit the screen saver
 * Enables applications to inhibit suspend actions
 * Sleep/Hibernate/Lock screen on lid action
 * Inhibit lid action if external monitor(s) is connected
 * Automatically suspend (sleep/hibernate)
 * Hibernate/Shutdown on critical battery
 * Configuration GUI
 * Implements org.freedesktop.ScreenSaver
 * Implements org.freedesktop.PowerManagement
 * Supports XScreenSaver

## Usage

powerdwarf is a user session daemon and should be started during the X11 startup session. If your desktop environment or window manager supports XDG auto start then powerdwarf should automatically start, if not you will need to add powerdwarf to your startup file (check the documentation included with your desktop environment or window manager).

 * In Fluxbox add ``powerdwarf &`` to the ``~/.fluxbox/startup`` file
 * In Openbox add ``powerdwarf &`` to the ``~/.config/openbox/autostart`` file.

## Configuration

powerdwarf can be configured through the included GUI or by manually editing the configuration file.

### GUI

Click on the powerdwarf system tray, or run the command (or use powerdwarf.desktop):
``` powerdwarf --config```

### Configuration file

You can also edit the settings through ``~/.config/powerdwarf/powerdwarf.conf``:

 * ``suspend_battery_timeout`` = ``<int>`` in min (suspend on battery after)
 * ``suspend_battery_action`` = ``<int>`` action taken when auto suspend on battery
   * ``0`` = ``none``
   * ``1`` = ``sleep``
   * ``2`` = ``hibernate``
   * ``3`` = ``shutdown``
 * ``suspend_ac_timeout`` = ``<int>`` in min (suspend on AC after)
 * ``suspend_ac_action`` = ``<int>`` action taken when auto suspend on AC
   * ``0`` = ``none``
   * ``1`` = ``sleep``
   * ``2`` = ``hibernate``
   * ``3`` = ``shutdown``
 * ``critical_battery_timeout`` = ``<int>`` in % (critical batter percent)
 * ``critical_battery_action`` = ``<int>`` action taken when battery is critical
   * ``0`` = ``none``
   * ``1`` = ``hibernate``
   * ``2`` = ``shutdown``
 * ``lid_battery_action`` = ``<int>`` action taken when lid is closed on battery
   * ``0`` = ``none``
   * ``1`` = ``lock``
   * ``2`` = ``sleep``
   * ``3`` = ``hibernate``
   * ``4`` = ``shutdown``
 * ``lid_ac_action`` = ``<int>`` action taken when lid is closed on AC
   * ``0`` = ``none``
   * ``1`` = ``lock``
   * ``2`` = ``sleep``
   * ``3`` = ``hibernate``
   * ``4`` = ``shutdown``
 * ``disable_lid_action_external_monitor`` = ``<bool>`` true/false (disable lid action if external monitor is connected)
 * ``freedesktop_ss`` = ``<bool>`` true/false (enable org.freedesktop.ScreenSaver)
 * ``freedesktop_pm`` = ``<bool>`` true/false (enable org.freedesktop.PowerManagement)
 * ``tray_notify`` = ``<bool>`` true/false (show notifications)
 * ``show_tray`` = ``<bool>`` true/false (show system tray)
 * ``icon_theme`` = ``<string>`` valid icon theme name (fallback)

You do not need to restart powerdwarf after editing the configuration file, powerdwarf will auto-detect if the file has changed.

## Requirements

powerdwarf requires the following dependencies to work:

### Build dependencies

 * X11
 * Xss
 * Xrandr
 * QtDBus
 * QtXml
 * QtGui
 * QtCore

### Run-time dependencies

 * ConsoleKit (or logind)
 * UPower 0.9.23 (or higher, note that 0.99.x requires logind)
 * XScreenSaver
 * adwaita-icon-theme (or similar)

Tested on Slackware, Ubuntu and FreeBSD.

### Icons

powerdwarf does not bundle any icons, so you will need a compatible theme installed (in ``share/applications``). powerdwarf will use the existing icon theme from your running DE/WM or will else fallback to known themes:

 * Adwaita
 * gnome
 * oxygen
 * Tango
 
 The following icons are required:
 
 * battery
 * battery-caution
 * battery-caution-charging
 * battery-low
 * battery-low-charging
 * battery-good
 * battery-good-charging
 * battery-full
 * battery-full-charging
 * battery-full-charged
 * battery-empty
 * battery-missing
 * video-display
 * system-suspend
 * preferences-other
 * user-available
 * system-lock-screen
 * system-hibernate
 * system-shutdown
 * emblem-unreadable

## Build

First make sure you have the required dependencies installed, then review the build options:

### Build options

 * **``PREFIX=</usr/local>``** : Install target.
 * **``XDGDIR=</etc/xdg>``** : Path to xdg autostart directory.
 * **``DOCDIR=<PREFIX/share/doc>``** : Path to the system documentation.
 * **``MANDIR=<PREFIX/share/man>``** : Path to the system manual.
 * **``UDEVDIR=</etc/udev>``** : Path to the udev directory (optional).
 * **``CONFIG+=release``** : Creates a release build
 * **``CONFIG+=no_doc_install``** : Do not install application documentation.
 * **``CONFIG+=no_man_install``** : Do not install application manual.
 * **``CONFIG+=no_desktop_install``** : Do not install the application desktop file.
 * **``CONFIG+=no_autostart_install``** : Do not install the XDG autostart desktop file.
 * **``CONFIG+=install_udev_rules``** : Install additional power related udev (backlight) rules

### Build application

```
mkdir build && cd build
qmake CONFIG+=release .. && make
```

Then just run ``app/powerdwarf`` or install with:

```
sudo make install
```

### Package application

```
qmake CONFIG+=release PREFIX=/usr
make
make INSTALL_ROOT=/pkg/path install
```
