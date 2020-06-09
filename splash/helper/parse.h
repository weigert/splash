namespace parse{
using namespace std;

  //Parse Data Struct
  struct data{

    unordered_map<string, bool> flags;
    unordered_map<string, bool> pflags;
    unordered_map<string, string> params;

    int x = 0, y = 0, w = 1920, h = 1080;

    bool program = false;
    std::string prog;

  } in;


  void input(int n, int ac, char* as[]){
    if(n == ac) return;

    string arg = string(as[n]);

    //Flag Argument
    if(arg.substr(0, 2) == "--" && arg.length() > 2)
      in.flags[arg] = true;

    //Parameter Argument
    else if(arg.substr(0, 1) == "-" && arg.length() > 1){
      int k = n + 1;
      if(k == ac) //out of bounds
        return;

      string p = "";
      for(int b = 0; k + b < ac; b++){
        in.pflags[arg] = true;
        p = string(as[k+b]);
        if(p.substr(0, 1) == "-")
          break;
        in.params[arg+to_string(b)] = p;
        n = k + b;
      }
    }

    input(++n, ac, as);
  }

  //Master Fetch
  void input(int ac, char* as[]){
    if(ac < 2) return;
    in.prog = std::string(as[1]);
    in.program = true;

    input(2, ac, as);

    //Do some necessary parsing
    if(in.pflags["-p"])
      in.x = stoi(in.params["-p0"]);
    if(in.pflags["-p"])
      in.y = stoi(in.params["-p1"]);
    if(in.pflags["-p"])
      in.w = stoi(in.params["-p2"]);
    if(in.pflags["-p"])
      in.h = stoi(in.params["-p3"]);

    logger::verbose = in.flags["--v"];
  }

};
