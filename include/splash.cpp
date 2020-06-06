#include <functional>
#include <initializer_list>
using Handle = std::function<void()>;
using slist = std::initializer_list<std::string>;

#include <GL/glew.h>                                //Rendering Dependencies

#include <sstream>                                  //File / Console IO
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

#include "X11/Xwindow.cpp"
#include "X11/Xevent.cpp"

#include <SDL2/SDL_image.h>

#include "utility/model.cpp"
#include "utility/shader.cpp"
#include "utility/texture.cpp"

#include "helper/image.h"
#include "helper/parse.h"
#include "helper/logger.h"
#include "helper/config.h"

#include "program/program.cpp"

int main( int argc, char* args[] ) {

	//Parse Input Arguments
	parse::input(argc, args);

	if(parse::in.verbose) logger::verbose = true;

	if(parse::in.test) config::test();

	if(parse::in.timeout)
		logger::write("Setting timeout", parse::in.t);

	if(!parse::in.pdata){
		logger::err("incomplete position data");
		return 0;
	}

	//Get Piped-Input!
	/*
	std::string test;
	std::cin >> test;
	*/

	//Construct the Splash According to Specification
	Splash splash(parse::in.x, parse::in.y, parse::in.w, parse::in.h);
	splash.interact = parse::in.interact;
	splash.background = parse::in.bg;
	splash.all = parse::in.all;
	splash.setup();

	//Setup our Rendering Properties!
	glewExperimental = GL_TRUE;     //Launch GLEW
	glewInit();
	glLineWidth(1.0);
	glViewport(0, 0, parse::in.w, parse::in.h);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	/*
		This is dependent on the actual program!
	*/

	//Program Definition
  //Image prog(test);

	//Fidget Program
	Fidget prog(parse::in.w, parse::in.h);

	auto start = std::chrono::high_resolution_clock::now();

	//Acutal Event Loop
	while (event::input(splash)) {

		if(parse::in.timeout){
			auto cur = std::chrono::high_resolution_clock::now();
			int s = std::chrono::duration_cast<std::chrono::seconds>(cur - start).count();
			if(s > parse::in.t){
				logger::write("Killing splash");
				break;
			}
		}

		(prog.pipeline)();
		(prog.event)();

		glXSwapBuffers(splash.Xdisplay, splash.gWindow);
	}

	return 0;
}
