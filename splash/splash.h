#include <string>
#include <chrono>
#include <vector>
#include <unistd.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

#include "helper/logger.h"
#include "helper/parse.h"
#include "helper/pipe.h"

#include "include/window.cpp"
#include "include/event.cpp"
#include "include/program.cpp"

/*
    Splash 2020
    Author: Nicholas McDonald
*/

//lmao
void printhelp(){
  logger::raw("splash 2020 by Nicholas McDonald");
}
