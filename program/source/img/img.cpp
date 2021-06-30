#include "../../program.h"
#include "img.h"

class Image : public Program {
private:

  //Utility Classes
  Square2D flat;
  Shader* shader;
  Texture* tex;

  //Shader Sources
  const std::string vs_source =
    #include "img.vs"
  ;
  const std::string fs_source =
    #include "img.fs"
  ;

public:

  //Constructor
  Image(svec* s):Program(s){
    shader = new Shader({vs_source, fs_source}, {"in_Quad, in_Tex"}, true);

    if(s == NULL){
      logger::err("No Image Specified");
      exit(0);
    }

    tex = new Texture(image::load(data->back()));
  }

  virtual void pipeline(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader->use();
    shader->uniform("model", flat.model);
    shader->texture("imageTexture", *tex);
    flat.render();
  }

  virtual void event(event::eventdata& e){
    /* ... */
  }

  virtual void onpipe(std::string s){
    /* ... */
  }

  ~Image(){
      delete shader;
      delete tex;
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
