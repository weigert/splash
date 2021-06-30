#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>

using svec = std::vector<std::string>;

#include <dlfcn.h>

#ifndef SPLASH_PROGRAM
#define SPLASH_PROGRAM

class Program {
public:

  Program(svec* s){

    data = s;

    ppipe = (root/"pipe"/spipe::next()).string();
    mkfifo(ppipe.c_str(), 0666);

    fpipe = open(ppipe.c_str(), O_RDONLY | O_NONBLOCK);
    if(fpipe < 0) logger::fatal("Failed to Open Pipe");
    logger::raw(ppipe);

  }

  virtual ~Program(){

    delete data;
    close(fpipe);
    if(remove(ppipe.c_str()) != 0)
      logger::raw("Failed to delete pipe");

  }

  svec* data;
  std::string ppipe;
  int fpipe;          //Pipe File Handle

  virtual void pipeline() = 0;                  //Rendering Pipeline
  virtual void event(event::eventdata&) = 0;    //Event Handler
  virtual void onpipe(std::string) = 0;         //Piped Data Handler

  static void* hndl;
  static Program* get(std::string, svec* s, parse::data* d);
  static void destroy(Program* p);

};

// Constructor and Destructor Factories

void* Program::hndl;
typedef Program* (*create_t)(svec* s, parse::data* d);
typedef void destroy_t(Program*);

Program* Program::get(std::string f, svec* s, parse::data* d){

	hndl = dlopen(f.c_str(), RTLD_NOW);
  if(hndl == NULL){
    std::cout<<dlerror()<<std::endl;
    return NULL;
  }

  create_t t = (create_t)dlsym(hndl, "create");
	const char *dlsym_error = dlerror();
	if (dlsym_error) {
    logger::err(dlsym_error);
    dlclose(hndl);
    return NULL;
  }

	return t(s, d);
}

void Program::destroy(Program* p){

  destroy_t* destroyer = (destroy_t*)dlsym(hndl, "destroy");
	const char *dlsym_error = dlerror();
	if (dlsym_error) {
		cerr << "Cannot load symbol destroy: " << dlsym_error << '\n';
		return;
	}

  destroyer(p);
}

#endif
