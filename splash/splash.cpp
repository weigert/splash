#include "splash.h"

int main( int argc, char* args[] ) {

	parse::input(argc, args);
	if(!parse::in.program){
		logger::err("No program");
		return 0;
	}

	if(parse::in.flags["--help"]){
		printhelp();
		return 0;
	}

	std::vector<string>* lines;
	if(spipe::available())
		lines = spipe::read();

	Splash splash(parse::in);

	const char * home = getenv ("HOME");
	if(home == NULL)
		logger::fatal("Home environment variable not set");
	fs::path fp = fs::path(home);
	Program* p = Program::get((fp/".config/splash/exec"/parse::in.prog).string(), lines, &parse::in);
	if(p == NULL)
		logger::fatal("Couldn't load program", parse::in.prog);

	auto start = std::chrono::high_resolution_clock::now();

	while(event::active){

		event::input(splash);

		if(parse::in.params["-t"]){
			auto cur = std::chrono::high_resolution_clock::now();
			int s = std::chrono::duration_cast<std::chrono::seconds>(cur - start).count();
			if(s > parse::in.params["-t"]){
				logger::write("Killing splash");
				event::active = false;
			}
		}

		p->pipeline();					//Execute the Program
		p->event(event::data);	//Program Event Handling

		event::handle();

		glXSwapBuffers(splash.Xdisplay, splash.gWindow);

	}

	delete p;
	return 0;
}
