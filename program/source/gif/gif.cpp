#include "../../program.h"
#include "gifwrap.c"
#include <unistd.h>

class Image : public Program {
private:

  //Utility Classes
  Square2D flat;
  Shader* shader;
  GIF_Image* gif;
  std::vector<Texture*> tex;
  int index = 0;

  //Shader Sources
  const std::string vs_source =
    #include "gif.vs"
  ;
  const std::string fs_source =
    #include "gif.fs"
  ;

public:

  //Constructor
  Image(svec* s):Program(s){
    shader = new Shader({vs_source, fs_source}, {"in_Quad, in_Tex"}, true);

    if(s == NULL){
      logger::err("No Image Specified");
      exit(0);
    }

    gif = GIF_LoadImage(data->back().c_str());
    for(int i = 0; i < gif->num_frames; i++)
      tex.emplace_back(new Texture(gif->frames[i]->surface));

  }

  virtual void pipeline(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();
    shader->uniform("model", flat.model);
    shader->texture("imageTexture", *tex[index]);
    flat.render();

    usleep(1000*gif->frames[index]->delay);
    index = (index + 1)%gif->num_frames;
  }

  virtual void event(event::eventdata& e){
    /* ... */
  }

  ~Image(){
      delete shader;
      for(auto& t: tex)
        delete t;
  }
};

extern "C" {
  Program* create(svec* s, parse::data* d) {
      return new Image(s);
  }

  void destroy(Program* p) {
      delete p;
  }
}
