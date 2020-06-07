# splash
Unix Splash Screen Tool

Made in C++

Tested on Ubuntu18 with Openbox

## Description
splash is a generic tool that provides a command-line interface for rendering non-windowed raw data directly onto your desktop environment.

splash transforms your entire desktop into a 2D / 3D canvas, for dynamic data or simple static overlays that can interact with your mouse and keyboard and trigger commands.

![fidget.gif](https://github.com/weigert/splash/blob/master/screenshots/fidget.gif)

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

    Options:

      -p [x y w h]  Position Splash Quad
      -t [s]        Splash timeout in seconds (integer)

    Flags:

      --t     Test configuration file
      --v     Verbose mode

      --bg    Place splash in background
      --fg    Place splash in foreground (default)
      --ni    Splash no-interact
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

If there is a desire for more detailed information on how to build a custom visualization, I can update the Wiki (if I get requests).

## Notes

### To-Do
- Proper configuration file system, to expose the programs to static user settings
- More well tested execution modes
  - Plotting methods
  - .obj file load and display
  - Particle system
  - GIF display
- Dynamic data streams by active pipe listening

### Why?
This was originally written because I realized I could make transparent windows in X11 with an OpenGL3 context. So implemented a desktop fidget spinner. Check it out with:

    splash fidget

When I also realized that using the C library dlopen I could decouple the actual visualization method from both the data AND the window, I decided to make splash.

### A terrible Idea

One could theoretically add a whole DearImGUI menu to your desktop using splash. Why? I don't know. But it's possible. Everyday I stray further from the light.

### Contributing
If you find this interesting and would like to contribute that would be awesome. A number of places where contributing would be nice:

  - Designing use cases for different execution modes
  - Testing portability for other window managers / desktop environments

## License

MIT License
