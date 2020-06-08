# splash
Unix Splash Screen Tool - turn your desktop into a 2D / 3D canvas

Made in C++

See below for distro / DE compatbility

## Description
splash is a generic tool that provides a command-line interface for rendering non-windowed raw data directly onto your desktop environment.

splash transforms your entire desktop into a 2D / 3D canvas, for dynamic data or simple static overlays that can interact with your mouse and keyboard and trigger commands.

![smbsplash.gif](https://github.com/weigert/splash/blob/master/screenshots/smbsplash.gif)

Example of splash displaying an animated GIF on the desktop background.

![fidget.gif](https://github.com/weigert/splash/blob/master/screenshots/fidget.gif)

Example of splash displaying a full 3D OpenGL application (albeit a very basic one) with user interaction.

This was originally written because I wanted to have a custom "splash screen" like those fancy, super expensive software packages have when starting up, with transparency and everything. Now it supports a full OpenGL context.

## Usage

Splash requires specification of an execution mode.

    Reading:
      []      Required
      <>      Optional

    <data |> splash [mode] <options/flags>

    Modes (built-in):

      help      Display help message

      fidget    Display a 3D fidget spinner (lol)
      <data>:   None

      img       Display .png image
      <data>:   Image file name

      gif       Display .gif image
      <data>:   GIF file name
      Note:     Only works for full replacing gifs!

    Options:

      -p [x y w h]  Position Splash Quad
      -t [s]        Splash timeout in seconds (integer)

    Flags:

      --t     Test configuration file
      --v     Verbose mode

      --bg    Place splash in background
      --fg    Place splash in foreground (default)
      --ni    Splash no-interact
      --ns    Disable splash shadows (compton)
      --a     Display splash on all desktops

## Installation

### Dependencies
Note that this requires X11.

  - g++
  - OpenGL, glX, GLEW
  - X11, Xfixes, Xrender
  - boost_system, boost_filesystem

### Setup

Run the script `setup.sh` with privileges.

The script will make sure that the required directories exist and will setup splash at `~/.config/splash`.

The script will also ask if you wish to compile splash and the execution modes.

The execution modes are placed in `~/.config/splash/exec` and splash is placed in `/usr/local/bin`.

### Compiling

If you wish to compile manually, use the makefiles in `splash/Makefile` and `program/Makefile`:

    ./splash:
      make splash

    ./program:
      make [mode]
      make all

Note that splash is separate from the actual execution modes. Execution modes are compiled separately (linked at runtime by splash).

### Compilation Issues

If you have problems with compiling search the closed issues to see if there is a solution and otherwise feel free to open a ticket.

Common problems might include: Incorrect linking in the make files, because your distro places libraries in a different location, and slightly different names of the libraries in `#include` directives.

## Compabitibility

This is just from some basic tests I can run on my computer. If you can compile / test on other distros and DEs, please open an issue so I can add it here.

Distros:

        Ubuntu 18           Compilation works
        Arch / Manjaro      Compilation works (fixable, see issues)
        ...                 Feel free to open an issue for your distro!

Desktop Environments:

        Gnome / Ubuntu      Works fully
        Openbox             Works fully
        XFCE                Works fully
        bspwm               Works fully

        i3                  *Restricted (see below)*
        i3-gaps             *Similar to i3*

        ...                 Feel free to open an issue for your DE!

The issues with the various distros are explained below.

The various degrees of compatibility depend on how strictly the window manager conforms to the X11 extended window manager hints specification ([EWMH](https://specifications.freedesktop.org/wm-spec/wm-spec-latest.html)).

Oftentimes, this can be mitigated by by setting up the WM configuration correctly.

### Congi

### Sidenote: compton shading
splash requires a compositor. If you use compton, splash screens will be default have shadows even with a transparent background. Shadows can be toggled using the `--ns` flag if you edit your `~/.config/compton.conf`:

    #Enable shadow toggling
    shadow-exclude = [
      #...
      "_COMPTON_SHADOW_OFF@:32c = 0",
      #...
    ];
    #...__

If you don't add this, compton **will always** shade splashes, and it can't be deactivated with `--ns`.

### i3 and i3-gaps
Both of these window managers do not support the EWMH specification for specifying a preferred order of floating windows (specifically `_NET_WM_STATE_ABOVE` and `_NET_WM_STATE_BELOW`).

These DEs seemingly work with two separate layers of windows (tiled and floating). Therefore, the `--bg` and `--fg` options do not work as intended.

All options work correctly, except that the splash will never "disappear" behind tiled windows (because there is no "desktop"), and splashes will conflict with other floating windows.

If you never use other floating windows and use splash just for overlays, the program works as intended.

See also:

https://github.com/i3/i3/issues/3265

#### i3 config
Add this line to your i3 config to make splash compatible (removes borders for windows of type splash, i.e. `_NET_WM_WINDOW_TYPE_SPLASH`):

    # splash config       
    for_window [window_type="splash"] border pixel 0

## Customization & How it Works

### config

I am currently working on a configuration system to make the display behavior more easily customizable.

Possible configuration options include color-schemes, transparency options and event triggers.

### Custom Mode

Currently, building custom execution modes requires knowledge of C++ and OpenGL. If you don't have the experience but you have an idea for a type of visualization you would like, let me know.

splash provides a "windowless"* OpenGL context and a system for modularized data visualization using small shader programs ("execution modes"), which have access to the piped-in data and user-input events.

    Note: An X11 window technically exists, it is just made "transparent" to the desktop.

A generic "program" base class is exposed to user input (piped data, commandline, X11 events). It contains an event handling callback and a rendering callback.

The program class is exposed to an OpenGL rendering context for visualization, as well as a number of utility classes that intuitively wrap boilerplate OpenGL (taken from [TinyEngine](https://github.com/weigert/TinyEngine) ).

By defining a derived class with specialized callbacks (and any additional desired members), arbitrary behavior can be achieved using the data piped in, on the "windowless" OpenGL window.

The derived classes are precompiled and placed into `~/.config/splash/exec` where they are found and linked by splash at run time.

You can build your own visualization programs by defining a custom derived class, compiling it and placing it in the `exec` folder. Check out the `program` folder for examples (as well as the makefile).

Interesting Reads:

https://www.linuxjournal.com/article/3687

https://www.tldp.org/HOWTO/html_single/C++-dlopen/

https://stackoverflow.com/questions/20443560/how-to-practically-ship-glsl-shaders-with-your-c-software

https://stackoverflow.com/questions/4052940/how-to-make-an-opengl-rendering-context-with-transparent-background

If there is a desire for more detailed information on how to build a custom visualization, I can update the Wiki (if I get requests).

## Notes

### To-Do
- Proper configuration file system, to expose the programs to static user settings
- More well tested execution modes
  - Plotting methods
  - .obj file load and display
  - Particle system
- Dynamic data streams by active pipe listening

### Why?

¯\_(ツ)_/¯

*check out my dope desktop fidget spinner, man*

    splash fidget

### A terrible Idea

One could theoretically add a whole DearImGUI menu to your desktop using splash. Why? I don't know. But it's possible. Everyday I stray further from the light.

### Contributing
If you find this interesting and would like to contribute that would be awesome. A number of places where contributing would be nice:

  - Designing use cases for different execution modes
  - Testing portability for other window managers / desktop environments

## License

MIT License
