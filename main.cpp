//#include "TinyEngine/TinyEngine.h"
//#include "TinyEngine/include/helpers/image.h"
#include <functional>
#include <initializer_list>
using Handle = std::function<void()>;
using slist = std::initializer_list<std::string>;

#include <GL/glew.h>                                //Rendering Dependencies

#include <sstream>                                  //File / Console IO
#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "splasher.cpp"
#include "event.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "utility/model.cpp"
#include "utility/shader.cpp"
#include "utility/texture.cpp"

#include "model.h"

int main( int argc, char* args[] ) {

	Splash splash;
	splash.makeWindow("Test Window");
	splash.makeContext();

	glewExperimental = GL_TRUE;     //Launch GLEW
	glewInit();
	glLineWidth(1.0);
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	setup();

	Model mesh(_construct);														//Construct a Mesh
	Shader defaultShader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal"});

	//What to Render
	Handle pipeline = [&](){

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		defaultShader.use();														//Prepare Shader
		defaultShader.uniform("model", mesh.model);			//Set Model Matrix
		defaultShader.uniform("vp", projection*camera);	//View Projection Matrix
		mesh.render(GL_LINES);													//Render Model with Lines

	};

	float rot[3] = {0.0f};
	float vrot[3] = {1.0f}; //Initial Velocity
	float trot[3] = {0.5f}; //Target Velocity ()

	event::handle = [&](){

		//Prepare Camera
		camera = glm::rotate(glm::mat4(1.0), glm::radians(rot[0]), glm::vec3(1, 0, 0));
		glm::vec4 newup = glm::inverse(camera)*glm::vec4(0.0f, 1.0f, 0.0f, 1.0);
		camera = glm::rotate(camera, glm::radians(rot[1]), glm::vec3(newup.x, newup.y, newup.z));

		if(event::clicked){
			//Accelerate
			vrot[1] += 0.05f*event::rx;
			vrot[0] += 0.05f*event::ry;
		}

		//Approach Goal Velocity
		vrot[0] += 0.05*(trot[0]-vrot[0]);		//Friction
		vrot[1] += 0.05*(trot[1]-vrot[1]);
		vrot[2] += 0.05*(trot[2]-vrot[2]);

		rot[0] += vrot[0];	//Speed
		rot[1] += vrot[1];
		rot[2] += vrot[2];

	};

	while (event::input(splash)) {

		(pipeline)();

		glXSwapBuffers(splash.Xdisplay, splash.glX_window_handle);

		(event::handle)();

	}

	return 0;
}
