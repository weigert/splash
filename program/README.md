# splash program
splash itself provides the constructing of the raw "splash screen" OpenGL context.

It also provides an interface for using the splash screen with your own custom shader programs via the "program" base class.

You can write your own program by writing a derived class and pre-compiling it. splash can find the program and link it dynamically at run time if it is placed in `~/.config/splash/exec`.

To see how to correctly write a derived class, check out these example modes.
