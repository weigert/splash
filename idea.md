## Splash Concept

Splash is similar to geektool, but it can do more things.

Splash is a generic tool for adding data visualization to your desktop environment.

Flags:
  --bg: Display in Background
  --fg: Display in Foreground
  --t:  Test Configuration File
  --v:  Verbose

Options:
  -p [x y w h]: Position data
  -t [s]:       Timeout (window kill time in seconds)
  -m [mode]:    Visualization Program
    mode = img, plot, surf, model
  -e [event]:   Event Callback

Usage:
  1. Pipe data into splash, e.g. a list of filenames, numbers, etc.
  2. Specify a visualization program
  3. Specify
  4. Set position and size of the splash
  5. (Optional) set other flags and options

To-Do:
  - Proper Modularization Concept for Visualization Programs
    - Include dynamic data streams for updating visualizations
      - Maybe with pipes? i.e. named pipes provide data streams
  - Program Types: plots of various kind, 3D objects, point clouds
    - define programs somehow in a config file...
    - how is the actual program source defined though? tricky.
  - Fix the piping system, so data is always PIPED in
    - i.e. make the data fully decoupled from the visualization
    - every line corresponds to a piece of data for visualization
  - Add a system for executing code based on interaction
    - modularized event handler independent of the visualization
    - a system for passing data along if required?
    - I want to click on a splash and spawn another splash or execute code
  - Make the programs somehow in their config have relevant variables
    - e.g. color schemes, transparency options, interaction triggers, etc.

Open Questions:
  How can I make it easier to build custom programs?

  The programs are currently very simple, but could this be made more easy to build programs somehow, without having to resort to weird programming paradigms / designing a weird scripting language.

  These programs use shaders and micro functions,
  that include an input handler and a visualizer function.

  What if I made it use .obj files or something?

  This might be possible somehow.

  I guess I could make a program folder that includes program XML files.
  And a way to construct these shader programs purely from XML files.
  WEIRD

  I suppose it would be possible. It would also be possible to just
  have a method for just taking vertices as raw data and drawing them.

  how do you make it modular enough for those purposes though?

  If it were somehow possible to make a c++ program that can be included in splash, that would be dope. That would be ideal.

  All we really need is to define two things:
    1. An event handler
    2. A rendering pipeline

  The rest does not matter and is handled by splash.
