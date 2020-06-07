namespace spipe {
  using namespace std;

  bool available(){
    return !isatty(fileno(stdin));
  }

  vector<string>* read(){
    vector<string>* lines = new vector<string>;
    string line;
    while (getline(cin, line)){
      lines->push_back(line);
      logger::write("Piped Input: ", line);
    }
    return lines;
  }
}
