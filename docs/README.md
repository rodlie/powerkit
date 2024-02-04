# NAME

*powerkit* - desktop independent Linux power manager

# SYNOPSIS

powerkit *`[--config]`*

# DESCRIPTION

powerkit is an desktop independent Linux power manager for alternative X11 desktop environments and window managers.

 * Implements *``org.freedesktop.ScreenSaver``* service and screen saver
 * Implements *``org.freedesktop.PowerManagement.Inhibit``* service
 * Automatically suspend on idle
 * Automatically suspend or shutdown on critical battery
 * Inhibit lid action if external monitor connected
 * Screen locking support
 * Screen backlight support
 * RTC wake support
 * Notification support (can use *``org.freedesktop.Notifications``* if available)

# USAGE

powerkit should be started during the X11 user session. Consult the documentation for your desktop environment or window manager for launching startup applications.

 * In *Fluxbox* add *``powerkit &``* to the *``~/.fluxbox/startup``* file
 * In *Openbox* add *``powerkit &``* to the *``~/.config/openbox/autostart``* file.

**Do use powerkit if your desktop environment or window manager already has a power manager or screen saver service running.**

## CONFIGURATION

The most common options are available directly from the system tray icon, for more advanced options open *``Settings``* from the system tray menu or run *``powerkit --config``*. You should also be able to lauch the powerkit settings from your desktop application menu (if available).

## SCREEN SAVER

powerkit implements a basic screen saver to handle screen blanking, poweroff and locking feature.

Locking feature depends on ``xsecurelock``.

You can override the lock command with *``screensaver_lock_cmd=<command>``* in *`~/.config/powerkit/powerkit.conf`*. Note that the command must not contain spaces.

## BACKLIGHT

The current display brightness (on laptops and supported displays) can be adjusted with the mouse wheel on the system tray icon or through the system tray menu.

## HIBERNATE

If hibernate works depends on your system, a swap partition (or file) is needed by the kernel to support hibernate.

***Consult your system documentation regarding hibernation***.

## ICONS

powerkit will use the existing icon theme from the running desktop environment or window manager.

You can override the icon theme in the *`~/.config/powerkit/powerkit.conf`* file, use *``icon_theme=<theme_name>``*.

# FAQ

## How does an application inhibit the screen saver?

The preferred way to inhibit the screen saver from an application is to use the *org.freedesktop.ScreenSaver* specification. Any application that uses *org.freedesktop.ScreenSaver* will work with powerkit.

Popular applications that uses this feature is Mozilla Firefox, Google Chrome, VideoLAN VLC and many more.

## How does an application inhibit suspend actions?

The preferred way to inhibit suspend actions from an application is to use the *org.freedesktop.PowerManagement* specification. Any application that uses *org.freedesktop.PowerManagement* will work with powerkit.

Common use cases are audio playback, downloading, rendering and similar.

## Google Chrome/Chromium does not inhibit the screen saver or power manager!?

*[Chrome](https://chrome.google.com)* does not use *org.freedesktop.ScreenSaver* or *org.freedesktop.PowerManagement* until it detects a supported desktop environment. Add the following to *``~/.bashrc``* or the *``google-chrome``* launcher if you don't run a supported desktop environment:

```
export DESKTOP_SESSION=xfce
export XDG_CURRENT_DESKTOP=xfce
```

# REQUIREMENTS

powerkit requires the following dependencies:

 * *[X11](https://www.x.org)*
 * *[libXss](https://www.x.org/archive//X11R7.7/doc/man/man3/Xss.3.xhtml)*
 * *[libXrandr](https://www.x.org/wiki/libraries/libxrandr/)*
 * *[Qt](https://qt.io)* 5.15 *(Core/DBus/Gui/Widgets)*
 * *[logind](https://www.freedesktop.org/wiki/Software/systemd/logind/)* *(or compatible service)*
 * *[UPower](https://upower.freedesktop.org/)* *(or compatible service)*
 * *[xsecurelock](https://github.com/google/xsecurelock)*

# BUILD

First make sure you have the required dependencies installed, then review the most common build options:

 * *``CMAKE_INSTALL_PREFIX=</usr/local>``* - Install target. *``/usr``* recommended.
 * *``CMAKE_BUILD_TYPE=<Release/Debug>``* - Build type. *``Release``* recommended
 * *``SERVICE_USER=<root>``* - powerkitd owner, needs write access to /sys. Usually the *``root``* user.
 * *``SERVICE_GROUP=<power>``* - Group that can access the powerkitd service, this should be any desktop user. Consult your system documentation for the preferred user group.

Now configure powerkit with CMake and build (*example for packaging purposes*).

```
cmake -DCMAKE_INSTALL_PREFIX=/usr -DSERVICE_GROUP=plugdev -DCMAKE_BUILD_TYPE=Release ..
make
make DESTDIR=<package> install
```
```
pkg
|-- etc
|   |-- dbus-1
|   |   `-- system.d
|   |       `-- org.freedesktop.PowerKit.conf
|   `-- xdg
|       `-- autostart
|           `-- powerkit.desktop
`-- usr
    |-- bin
    |   `-- powerkit
    |-- libexec
    |   `-- powerkitd
    `-- share
        |-- applications
        |   `-- powerkit.desktop
        |-- dbus-1
        |   `-- system-services
        |       `-- org.freedesktop.PowerKit.service
        |-- doc
        |   `-- powerkit-VERSION
        |       |-- LICENSE
        |       `-- README.md
        `-- man
            |-- man1
            |   `-- powerkit.1
            `-- man8
                `-- powerkitd.8

```

# CHANGELOG

## 2.0.0 (TBA)

 * Recommended locker is ``xsecurelock``
 * Improved support for logind
 * Removed support for ConsoleKit
 * Removed support for XScreenSaver
 * Added screen saver in powerkit
 * Easier to use (minimal setup)
 * New UI
 * RTC wake alarm support (not used for anything yet)
   * Hibernate computer while suspended for X amount of time
 * CPU frequency scaling and thermal support (not used for anything yet)
   * Intel PState
 * powerkitd
   * Service for unprivileged users (needed for CPU/RTC/brightness)

May change at any time during development.

# OPTIONS

*``--config``*
: Launch configuration.

# FILES

*``~/.config/powerkit/powerkit.conf``*
: Per user configuration file.

# SEE ALSO

**``xsecurelock``**(1), **``UPower``**(7), **``powerkitd``**(8)

# BUGS

See **https://github.com/rodlie/powerkit/issues**.

# COPYRIGHT

```
Copyright (c) Ole-André Rodlie <https://github.com/rodlie>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
