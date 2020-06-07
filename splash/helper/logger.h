#include <iostream>

namespace logger{
  bool verbose = false;
  bool debug = true;

  template <typename T>
  void raw(T m){
    std::cout<<m<<std::endl;
  }

  template <typename T, typename... Types>
  void raw(T m, Types... r){
    std::cout<<m<<" ";
    raw(r...);
  }

  template <typename T>
  bool err(T m){
    if(!debug) return false;
    std::cout<<"Error: ";
    raw(m);
    return false;
  }

  template <typename T, typename... Types>
  bool err(T m, Types... r){
    if(!debug) return false;
    std::cout<<"Error: ";
    raw(m, r...);
    return false;
  }

  template <typename T>
  void fatal(T m){
    std::cout<<"Fatal Error: ";
    raw(m);
    exit(0);
  }

  template <typename T, typename... Types>
  void fatal(T m, Types... r){
    std::cout<<"Fatal Error: ";
    raw(m, r...);
    exit(0);
  }

  template <typename T>
  void write(T m){
    if(!verbose) return;
    std::cout<<"Splash: ";
    raw(m);
  }

  template <typename T, typename... Types>
  void write(T m, Types... r){
    if(!verbose) return;
    std::cout<<"Splash: ";
    raw(m, r...);
  }
}
