#include "splash.h"

int main( int argc, char* args[] ) {

	parse::input(argc, args);
	if(!parse::in.program){
		logger::err("No program");
		return 0;
	}

	if(parse::in.prog == "help"){
		logger::raw(help);
		return 0;
	}

	if(parse::in.prog == "info"){
		logger::raw(info);
		return 0;
	}

	vector<string>* lines = new vector<string>;
	if(spipe::available())
		spipe::read(lines);

	Splash splash(parse::in);

	const char * home = getenv ("HOME");
	if(home == NULL)
		logger::fatal("Home environment variable not set");
	fs::path fp = fs::path(home);

	Program* p = Program::get((fp/".config/splash/exec"/parse::in.prog).string(), lines, &parse::in);
	if(p == NULL)
		logger::fatal("Couldn't load program", parse::in.prog);

	auto start = std::chrono::high_resolution_clock::now();

	signal(SIGINT, &sighandler);

	while(event::active && p->alive){

		event::input(splash);

		if(parse::in.pflags["-t"]){
			auto cur = std::chrono::high_resolution_clock::now();
			int s = std::chrono::duration_cast<std::chrono::seconds>(cur - start).count();
			if(s > stoi(parse::in.params["-t0"])){
				logger::write("Killing splash");
				event::active = false;
			}
		}

		p->pipeline();					//Execute the Program
		p->event(event::data);	//Program Event Handling

		std::string line = spipe::fifo(p->fpipe);
		if(!line.empty()) p->onpipe(line);

		event::handle();

		glXSwapBuffers(splash.Xdisplay, splash.gWindow);
	}

	Program::destroy(p);
	return 0;
}
