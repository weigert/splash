#include "../splash/splash.h"

#include <initializer_list>
#include <functional>
#include <sstream>
#include <fstream>

using Handle = std::function<void()>;
using slist = std::initializer_list<std::string>;
using svec = std::vector<std::string>;

#include <SDL2/SDL_image.h>

#include "utility/texture.cpp"
#include "utility/model.cpp"
#include "utility/instance.cpp"
#include "utility/shader.cpp"
#include "utility/target.cpp"
