# PowerKit

powerkit is a lightweight full featured power manager created for Slackware for use with alternative desktop environments and window managers, like Fluxbox, Blackbox, FVWM, WindowMaker, Openbox, Lumina and others.

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
 * Back light support

## Usage

powerkit is a user session daemon and should be started during the X11 startup session. If your desktop environment or window manager supports XDG auto start then powerkit should automatically start, if not you will need to add powerkit to your startup file (check the documentation included with your desktop environment or window manager).

 * In Fluxbox add ``powerkit &`` to the ``~/.fluxbox/startup`` file
 * In Openbox add ``powerkit &`` to the ``~/.config/openbox/autostart`` file.

## Configuration

Click on the powerkit system tray, or run the command ``` powerkit --config``` (or use powerkit.desktop) to configure powerkit.

### Screen saver

powerkit depends on XScreenSaver to handle the screen session, the default settings may need to be adjusted. You can launch the configuration GUI with the ``xscreensaver-demo`` command.

Recommended settings are:

* Mode: ``Blank Screen Only``
* Blank After: ``5 minutes``
* Lock Screen After: ``enabled + 0 minutes``
* Display Power Management: ``enabled``
  * Standby After: ``0 minutes``
  * Suspend After: ``0 minutes``
  * Off After: ``0 minutes``
  * Quick Power-off in Blank Only Mode: ``enabled``

Note that powerkit will start XScreenSaver during startup (unless ``freedesktop_ss`` is disabled).

### Back light

powerkit supports back light only on Linux through ``/sys/class/backlight``. The current brightness can be adjusted with the mouse wheel on the system tray icon or through the configuration GUI (bottom left slider).

**Note!** udev permissions are required to adjust the brightness, on Slackware an [example](https://github.com/rodlie/powerkit/blob/master/app/share/udev/90-backlight.rules) rule file is included with the package (see ``/usr/doc/powerkit-VERSION/90-backlight.rules``). You can also let powerkit add the rule during build with the ``CONFIG+=install_udev_rules`` option.

### Hibernate

A swap partition (or file) is needed by the kernel to support hibernate. Edit the boot loader (e)lilo/grub configuration and add the kernel option ``resume=<swap_partition/swap_file>``, then save and restart.

**Note!** some distributions have hibernate disabled (for Ubuntu see [com.ubuntu.enable-hibernate.pkla](https://github.com/rodlie/powerkit/blob/master/app/share/polkit/localauthority/50-local.d/com.ubuntu.enable-hibernate.pkla)).

## FAQ

### Slackware-only?

No, powerkit should work on any Linux/FreeBSD system (check requirements). However, powerkit is developed on/for Slackware and sees minimal testing on other systems (user feedback/bugs for other systems are welcome).

### How does an application inhibit the screen saver?

The preferred way to inhibit the screen saver from an application is to use the ``org.freedesktop.ScreenSaver`` specification. Any application that uses ``org.freedesktop.ScreenSaver`` will work with powerkit. Note that powerkit also includes ``SimulateUserActivity`` for backwards compatibility.

Popular applications that uses this feature is Mozilla Firefox (for audio/video), VideoLAN VLC and many more.

### How does an application inhibit suspend actions?

The prefered way to inhibit suspend actions from an application is to use the ``org.freedesktop.PowerManagement`` specification. Any application that uses ``org.freedesktop.PowerManagement`` will work with powerkit.

Common use cases are audio playback, downloading and more.

## Requirements

powerkit requires the following dependencies to work:

### Build dependencies

 * X11
 * Xss
 * Xrandr
 * QtDBus
 * QtGui
 * QtCore

### Run-time dependencies

 * ConsoleKit (or logind)
 * UPower 0.9.23(+)
 * XScreenSaver
 * xrandr
 * adwaita-icon-theme (or similar)

### Icons

powerkit does not bundle any icons, so you will need a compatible theme installed (in ``share/applications``). powerkit will use the existing icon theme from your running DE/WM or fallback to a known theme:

 * Adwaita
 * Gnome
 * Oxygen
 * Tango
 
## Build

First make sure you have the required dependencies installed, then review the build options:

### Build options

 * **``PREFIX=</usr/local>``** : Install target.
 * **``XDGDIR=</etc/xdg>``** : Path to xdg autostart directory.
 * **``DOCDIR=<PREFIX/share/doc>``** : Path to the system documentation.
 * **``MANDIR=<PREFIX/share/man>``** : Path to the system manual.
 * **``CONFIG+=release``** : Creates a release build
 * **``CONFIG+=no_doc_install``** : Do not install application documentation.
 * **``CONFIG+=no_man_install``** : Do not install application manual.
 * **``CONFIG+=no_desktop_install``** : Do not install the application desktop file.
 * **``CONFIG+=no_autostart_install``** : Do not install the XDG autostart desktop file.
 * **``CONFIG+=install_udev_rules``** : Install additional power related udev (backlight) rules
    * **``UDEVDIR=</etc/udev>``** : Path to the udev directory.
 * **``CONFIG+=install_lib``**: Build and install shared library.
    * **``CONFIG+=no_include_install``**: Do not install include files.
    * **``CONFIG+=no_pkgconfig_install``**: Do not install pkgconfig file.

### Build application

```
mkdir build && cd build
qmake CONFIG+=release .. && make
```

Then just run ``app/powerkit`` or install with:

```
sudo make install
```

### Package application

```
qmake CONFIG+=release PREFIX=/usr
make
make INSTALL_ROOT=pkg_path install
```
