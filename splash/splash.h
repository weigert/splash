#include <string>
#include <chrono>
#include <vector>
#include <unistd.h>
#include <csignal>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

const fs::path home = fs::path(getenv("HOME"));
const fs::path root = home/".config/splash";

#include "helper/logger.h"
#include "helper/parse.h"
#include "helper/pipe.h"

#include "include/window.cpp"
#include "include/event.cpp"
#include "include/program.cpp"

void sighandler(int signal){
  event::active = false;
}

const std::string info =
  #include "resource/info.txt"
;

const std::string help =
  #include "resource/help.txt"
;
