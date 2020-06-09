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
}
