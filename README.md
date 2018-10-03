# powerdwarf

powerdwarf is a lightweight full featured power manager created for Slackware for use with alternative desktop environments and window managers, like Fluxbox, Blackbox, FVWM, WindowMaker, Openbox, Lumina and others.

## Features

 * Enables applications to inhibit the screen saver
   * Implements org.freedesktop.ScreenSaver
 * Enables applications to inhibit suspend actions
   * Implements org.freedesktop.PowerManagement
 * Sleep/Hibernate/Lock screen on lid action
 * Inhibit lid action if external monitor(s) is connected
 * Automatically suspend (sleep/hibernate)
 * Hibernate/Shutdown on critical battery
 * Simple and flexible configuration GUI
 * XScreenSaver support
 * Backlight support

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
 * ``critical_battery_timeout`` = ``<int>`` in % (critical battery percent)
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
   * ``lid_xrandr_action`` = ``<bool>`` true/false (turn internal monitor on/off with xrandr when lid is triggered)
 * ``freedesktop_ss`` = ``<bool>`` true/false (enable org.freedesktop.ScreenSaver)
 * ``freedesktop_pm`` = ``<bool>`` true/false (enable org.freedesktop.PowerManagement)
 * ``tray_notify`` = ``<bool>`` true/false (show notifications)
 * ``show_tray`` = ``<bool>`` true/false (show system tray)
 * ``icon_theme`` = ``<string>`` valid icon theme name (fallback)

You will only need to restart powerdwarf if you edit ``freedesktop_ss``, ``freedesktop_pm`` or ``icon_theme``. Note that ``show_tray`` will not hide/show the system tray instantly, but the next time an event happens.

### Screen saver

powerdwarf depends on XScreenSaver to handle the screen session, the default settings may need to be adjusted. You can launch the configuration GUI with the ``xscreensaver-demo`` command.

Recommended settings are:

* Mode: ``Blank Screen Only``
* Blank After: ``5 minutes``
* Lock Screen After: ``enabled + 0 minutes``
* Display Power Management: ``enabled``
  * Standby After: ``0 minutes``
  * Suspend After: ``0 minutes``
  * Off After: ``0 minutes``
  * Quick Power-off in Blank Only Mode: ``enabled``
  
Note that powerdwarf will start XScreenSaver during startup (unless ``freedesktop_ss`` is disabled).

### Backlight

powerdwarf supports backlight only on Linux through ``sys/class/backlight``. The current brightness can be adjusted with the mouse wheel on the systram tray icon or through the configuration GUI (bottom left slider).

**Note!** udev permissions are required to adjust the brightness, on Slackware an example rule file is included with the package (see ``/usr/doc/powerdwarf-VERSION/90-backlight.rules``). You can also let powerdwarf add the rule during build with the ``CONFIG+=install_udev_rules`` option.

## FAQ

### Slackware-only?

No, powerdwarf should work on any Linux/FreeBSD system (check requirements). However, powerdwarf is developed on/for Slackware and sees minimal testing on other systems (user feedback/bugs for other systems are welcome).

### How does an application inhibit the screen saver?

The prefered way to inhibit the screen saver from an application is to use the ``org.freedesktop.ScreenSaver`` specification. Any application that uses ``org.freedesktop.ScreenSaver`` will work with powerdwarf. Note that powerdwarf also includes ``SimulateUserActivity`` for backwards compatibility.

Popular applications that uses this feature is Mozilla Firefox (for audio/video), VideoLAN VLC and many more.

### How does an application inhibit suspend actions?

The prefered way to inhibit suspend actions from an application is to use the ``org.freedesktop.PowerManagement`` specification. Any application that uses ``org.freedesktop.PowerManagement`` will work with powerdwarf.

Common use cases are audio playback, downloading and more.

### Hybrid suspend?

Not at this moment. Note that powerdwarf will trigger the critical battery action (hibernate/shutdown) even when suspended.

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
 * xrandr
 * adwaita-icon-theme (or similar)

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
 * weather-clear

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
make INSTALL_ROOT=pkg_path install
```
```
pkg/
├── etc
│   └── xdg
│       └── autostart
│           └── powerdwarf.desktop
└── usr
    ├── bin
    │   └── powerdwarf
    └── share
        ├── applications
        │   └── powerdwarf.desktop
        ├── doc
        │   └── powerdwarf-VERSION
        │       ├── LICENSE
        │       └── README.md
        └── man
            └── man1
                └── powerdwarf.1
```
