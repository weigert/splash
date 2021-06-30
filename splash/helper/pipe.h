namespace spipe {
  using namespace std;

  bool available(){
    if(!isatty(fileno(stdin)))
      return true;
    logger::write("No piped data");
    return false;
  }

  bool read(vector<string>* lines){
    string line;
    lines->clear();
    while (getline(cin, line)){
      lines->push_back(line);
      logger::write("Piped Input: ", line);
    }
    cin.clear();
    return lines->empty();
  }

  std::string next(){
    int n = 0;

    const char * home = getenv ("HOME");
    fs::path fp = fs::path(home);
    while(fs::exists(root/"pipe"/("pipe"+std::to_string(n))))
      n++;

    return ("pipe"+std::to_string(n));
  }

  std::string fifo(int fpipe){

    std::string line;
    char buf[256] = {""};

    int m = 0;
    int n = ::read(fpipe, buf, sizeof(buf));
    while(n > 0){
      line += buf;
      m += n;
      n = ::read(fpipe, buf, sizeof(buf));
    }

    return line.substr(0, m-1);

  }

}
