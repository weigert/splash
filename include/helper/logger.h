
namespace logger{

  template <typename T>
  void raw(T m){
    std::cout<<m<<std::endl;
  }

  template <typename T, typename... Types>
  void raw(T m, Types... r){
    std::cout<<m<<" ";
    raw(r...);
  }

  bool verbose = false;
  bool error = true;
  void err(std::string e){
    if(!error) return;
    std::cout<<"Error: "<<e<<std::endl;
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
