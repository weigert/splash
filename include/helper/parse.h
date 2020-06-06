namespace parse{

  //Parse Data Struct
  struct data{

    bool test = false;  //Test Configuration File
    bool verbose = false;
    bool bg = false;    //Background / Foreground
    bool pdata = false;
    int x, y, w, h;     //Position Data
    bool timeout = false;
    int t = -1;          //Timeout
    bool interact = true;
    bool all = false;

    bool inputfile = false;
    std::string file;   //Filename

  } in;


  void input(int n, int ac, char* as[]){
    if(n == ac) return;

    std::string arg = std::string(as[n]);
    int k = 1;

    //Flag Arguments

    if(arg == "--t") in.test = true;
    if(arg == "--v") in.verbose = true;
    if(arg == "--bg")   in.bg = true;
    if(arg == "--fg")   in.bg = false; //(default)
    if(arg == "--ni")   in.interact = false; //(default)
    if(arg == "--a")   in.all = true; //(default)

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

    if(arg == "-m"){
      if(ac - n > 1){ //Sufficient Args
        in.inputfile = true;
        in.file = std::string(as[n+1]);
        k++;
      }
      else std::cout<<"Please specify a filename"<<std::endl;
    }

    input(n+k, ac, as);
  }

  //Master Fetch
  void input(int ac, char* as[]){
    input(0, ac, as);
  }
};
