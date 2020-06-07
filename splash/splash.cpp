#include "splash.h"

int main( int argc, char* args[] ) {

	std::vector<string>* lines;
	if(spipe::available())
		lines = spipe::read();

	parse::input(argc, args);
	if(!parse::in.program){
		logger::err("No program");
		return 0;
	}

	Splash splash(parse::in);

	Program* p = Program::get("exec/"+parse::in.prog, lines);
	if(p == NULL){
		logger::err("Couldn't load program", parse::in.prog);
		return 0;
	}

	auto start = std::chrono::high_resolution_clock::now();

	while(event::active){

		event::input(splash);

		if(parse::in.timeout){
			auto cur = std::chrono::high_resolution_clock::now();
			int s = std::chrono::duration_cast<std::chrono::seconds>(cur - start).count();
			if(s > parse::in.t){
				logger::write("Killing splash");
				event::active = false;
			}
		}

		p->pipeline();					//Execute the Program
		p->event(event::data);	//Program Event Handling

		glXSwapBuffers(splash.Xdisplay, splash.gWindow);

	}

	return 0;
}
