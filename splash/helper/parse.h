namespace parse{

  //Parse Data Struct
  struct data{

    bool test = false;  //Test Configuration File
    bool bg = false;    //Background / Foreground
    bool pdata = false;
    int x = 0, y = 0,
    w = 1920, h = 1080;     //Position Data
    bool timeout = false;
    int t = -1;          //Timeout
    bool interact = true;
    bool all = false;
    bool shade = true;
    bool program = false;
    std::string prog;

  } in;


  void input(int n, int ac, char* as[]){
    if(n == ac) return;

    std::string arg = std::string(as[n]);
    int k = 1;

    //Flag Arguments
    
    if(arg == "--v")    logger::verbose = true;
    if(arg == "--t")    in.test = true;
    if(arg == "--bg")   in.bg = true;
    if(arg == "--fg")   in.bg = false;  //default
    if(arg == "--ni")   in.interact = false;
    if(arg == "--ns")   in.shade = false;
    if(arg == "--a")    in.all = true;

    //Data-Arguments

    if(arg == "-p"){ //Position data
      if(ac - n > 4){ //Sufficient Args
        in.pdata = true;
        in.x = atoi(as[n+1]);
        in.y = atoi(as[n+2]);
        in.w = atoi(as[n+3]);
        in.h = atoi(as[n+4]);
        k += 4;
      }
      else std::cout<<"Insufficient Arguments"<<std::endl;
    }

    if(arg == "-t"){ //Position data
      if(ac - n > 1){ //Sufficient Args
        in.timeout = true;
        in.t = atoi(as[n+1]);
        k++;
      }
      else std::cout<<"Insufficient Arguments"<<std::endl;
    }

    input(n+k, ac, as);
  }

  //Master Fetch
  void input(int ac, char* as[]){
    if(ac < 2) return;
    in.prog = std::string(as[1]);
    in.program = true;
    input(2, ac, as);
  }

};
