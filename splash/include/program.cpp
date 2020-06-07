#include <vector>
#include <string>
#include <functional>
#include <iostream>

using svec = std::vector<std::string>;

#include <dlfcn.h>

#ifndef PROGRAM_CPP
#define PROGRAM_CPP

class Program {
public:
  Program(svec* s){
    data = s;
  }

  virtual ~Program(){
    delete data;
  }

  svec* data;

  virtual void pipeline() = 0;
  virtual void event(event::eventdata&) = 0;

  static Program* get(std::string, svec* s, parse::data* d);
};

typedef Program* (*create_t)(svec* s, parse::data* d);
typedef void destroy_t(Program*);

Program* Program::get(std::string f, svec* s, parse::data* d){

	void *hndl = dlopen(f.c_str(), RTLD_NOW);
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

#endif
