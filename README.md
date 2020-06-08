# splash
Unix Splash Screen Tool

Made in C++

see below for distro / DE compatbility

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

### Note: Compton Shading
This program depends on the existence of a compositor. If you use compton as your compositor, you can edit your `compton.conf` to enable shadow toggling for the splashes:

    #Enable shadow toggling
    shadow-exclude = [
      #...
      "_COMPTON_SHADOW_OFF@:32c = 0",
      #...
    ];
    #...

Adding this allows you to use `--ns` to toggle shadows.
If you don't add this, then compton **will** shade splashes (if you have it activated), and it can't be deactivated for the splashes.
__

### Note: i3 configuration
Add this line to your i3 config to make splash compatible:

    # splash config       
    for_window [window_type="splash"] border pixel 0

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

### Compabitibility

This is just from some basic tests I can run on my computer. If you can compile / test on other distros and DEs, please open an issue so I can add it here.

Distros:

        Ubuntu 18           Compilation work
        Arch / Manjaro      Compilation incomplete

Desktop Environments:

        Gnome               Works fully
        Openbox             Works fully
        i3                  Broken (very)
                                Reason: i3 is very strict about how it places windows. I am working on a work-around.
        i3-gaps             Not tested
        bspwn               Not tested

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
