# splash
Unix Splash Screen Tool - turn your desktop into a 2D / 3D canvas

Made in C/C++ + XLIB

See below for distro / DE compatbility

## Description
splash is a generic tool that provides a command-line interface for rendering non-windowed raw data directly onto your desktop environment.

splash transforms your entire desktop into a 2D / 3D canvas, for dynamic data or simple static overlays that can interact with your mouse and keyboard and trigger commands.

![smbsplash.gif](https://github.com/weigert/splash/blob/master/screenshots/smbsplash.gif)

Example of splash displaying an animated GIF on the desktop background.

![fidget.gif](https://github.com/weigert/splash/blob/master/screenshots/fidget.gif)

Example of splash displaying a full 3D OpenGL application (albeit a very basic one) with user interaction.

![ttf.gif](https://github.com/weigert/splash/blob/master/screenshots/ttf.gif)

Example of splash rendering truetype font text, with a bunch of options, in the foreground on the desktop.

This was originally written because I wanted to have a custom "splash screen" like those fancy, super expensive software packages have when starting up, with transparency and everything. Now it supports a full OpenGL context.

### Features

splash rovides an elegant interface for creating a light-weight, "windowless" OpenGL context on your desktop. It also provides a simply system for accessing command line options and access to a pipe for streaming data to the visualization. Programs are dynamically linked so splash is very easy to extend (see below for details). The structure thereby allows for the development of light-weight, highly modular shader based desktop visuals.

    - dynamically linked, shader-based execution modes
    - super light code structure and execution cost
    - streaming data to exeuction modes via pipes
    - easy to develpo new execution modes

## Usage

splash requires specification of an execution mode, which can then use piped data as well as options:

    Reading:
      []      Required
      <>      Optional

    Running:
    <data |> splash [mode] <options/flags>

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

    Modes (included by default):

      help      Display help message (this message)
      info      Display info message

      fidget    Display a 3D fidget spinner (lol)
      hairy     A different fidget spinner
      spiky     Yet another different spinner

      img       Display .png image
      [data]:   Image file name

      gif       Display .gif image (note: only full replacing gifs)
      [data]:   GIF file name

      text      Render truetype text (static)
      [data]:   Text to render
        -ff [font]    Font face (searches in ~/.fonts/, default "arial.ttf")
        -fc [hex]     Text color (hex code, e.g. 0xFFFFFF)
        -fs [size]    Font size, positive integer
        -v  [0,1,2]   Vertical Align [center, up, down]
        -h  [0,1,2]   Horizontal Align [center, left, right]

### Data Piping

Data can be piped into a splash directly at the start, using e.g.:

    echo "splash" | splash text -p 100 100 800 400 --bg --ns  -ff "arial.ttf" -fs 100
    
Every splash program will also additionally open a numbered named pipe in `~/.config/splash/pipe/`, on which it (can) listen for data stream to affect the visualization. The name of the pipe is returned by splash:

    > splash text -p 100 100 800 400 --bg --ns  -ff "arial.ttf" -fs 100
    $ /home/user/.config/splash/pipe/pipe0
    
which can then be used to stream data from another source, e.g.:

    > watch -n1 'date +%H:%M:%S > /home/user/.config/splash/pipe/pipe0'

resulting in a clock visualized in the splash opened previously with the correct settings.

Note that what happens with the data depends on the programming of the execution mode.

### Examples

To test your installation of splash and see some application examples, check the `test` folder. It comes with some data provided.

## Installation

### Dependencies

    Compiler:         g++
    Rendering:        OpenGL, glX, GLEW
    X-Server:         X11, Xfixes, Xrender
    Boost:            boost, boost_system, boost_filesystem

All of these should be available as packages for your favorite distro.

### Setup

Run the script `setup.sh` **with privileges**.

The script will make sure that the required directories exist and will setup splash at `~/.config/splash`.

The script will also ask if you wish to compile splash and the execution modes.

The execution modes are placed in `~/.config/splash/exec` and splash is placed in `/usr/local/bin`.

#### Manual Compiling

If you wish to compile manually, use the makefiles in `splash/Makefile` and `program/Makefile`:

    # ./splash:
      make build
      sudo make install
    # or
      sudo make splash

    # ./program:
      make build
      make install
    # or
      make all

Note that splash is separate from the actual execution modes. Execution modes are compiled separately (linked at runtime by splash). Execution mode installation does not require privilege.

#### Compilation Issues

If you have problems with compiling search the closed issues to see if there is a solution and otherwise feel free to open a ticket.

Common problems might include: Incorrect linking in the make files, because your distro places libraries in a different location, and slightly different names of the libraries in `#include` directives.

## Compabitibility / Requirements / Configuration

This is just from some basic tests I can run on my computer. If you can compile / test on other distros and DEs, please open an issue so I can add it here.

Distros:

        Ubuntu 18           Compiles successfully
        Arch / Manjaro      Compiles successfully (see issues)
        ...                 feel free to open an issue for your distro!

Desktop Environments:

        Gnome / Ubuntu      Works fully
        Openbox             Works fully
        XFCE                Works fully
        bspwm               Works fully (requires config, see below)

        i3                  *Restricted (see below)*
        i3-gaps             Not tested

        ...                 Feel free to open an issue for your DE!

**Note**: Some WMs require additions to their config.

**splash wiki**: [window manager compatibility and system configuration](https://github.com/weigert/splash/wiki/Window-Manager-Configuration)

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
