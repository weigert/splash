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
  void err(T m){
    if(!debug) return;
    std::cout<<"Error: ";
    raw(m);
  }

  template <typename T, typename... Types>
  void err(T m, Types... r){
    if(!debug) return;
    std::cout<<"Error: ";
    raw(m, r...);
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
