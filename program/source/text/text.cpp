#include "../../program.h"
#include <SDL2/SDL_ttf.h>
#include <unistd.h>

class Text : public Program {
private:

  //Utility Classes
  Square2D flat;
  Shader* shader;
  Texture* tex;
  int fontsize = 18;
  string fontface = "font.ttf";

  int halign = 0; //0 = center, 1 = left, 2 = right
  int valign = 0; //0 = center, 1 = up, 2 = down

  glm::mat4 model = glm::mat4(1.0);

  std::vector<string> fontdirs = {
    "/usr/local/share/fonts"
  };

  //Shader Sources
  const std::string vs_source =
    #include "text.vs"
  ;
  const std::string fs_source =
    #include "text.fs"
  ;

public:

  //Constructor
  Text(svec* s, parse::data* d):Program(s){
    TTF_Init();

    shader = new Shader({vs_source, fs_source}, {"in_Quad, in_Tex"}, true);

    if(s == NULL){
      logger::err("No Text Specified");
      exit(0);
    }

    TTF_Font* font = TTF_OpenFont(fontface.c_str(), fontsize);
    if(!font) logger::fatal("Couldn't open font");

    SDL_Color colorf = { 0, 0, 0, 0 };
    SDL_Surface* surface = TTF_RenderText_Blended(font, s->back().c_str(), colorf);
    TTF_CloseFont(font);

    tex = new Texture();
    tex->raw(surface, [](Texture* t){ //Setup Tex Parameters Correctly
      glTexParameteri(t->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(t->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(t->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(t->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(t->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    });

    //Scale the text to font size
    glm::vec3 scale = glm::vec3((float)tex->W/(float)d->w, (float)tex->H/(float)d->h, 1.0);

    //Shift Text in Box
    glm::vec3 shift = glm::vec3(0); //Default: Centered

    if(halign == 1)       //Left
      shift.x -= ((float)d->w - (float)tex->W)/(float)d->w;
    else if(halign == 2)  //Right
      shift.x += ((float)d->w - (float)tex->W)/(float)d->w;

    if(valign == 1)       //Up
      shift.y += ((float)d->h - (float)tex->H)/(float)d->h;
    else if(valign == 2)  //Down
      shift.y -= ((float)d->h - (float)tex->H)/(float)d->h;

    model = glm::translate(model, shift);
    model = glm::scale(model, scale);

    SDL_FreeSurface(surface);
  }

  virtual void pipeline(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();
    shader->uniform("model", model);
    shader->texture("imageTexture", *tex);
    flat.render();
  }

  virtual void event(event::eventdata& e){
    /* ... */
  }

  ~Text(){
      delete shader;
      delete tex;
      TTF_Quit();
  }
};

extern "C" {
  Program* create(svec* s, parse::data* d) {
      return new Text(s, d);
  }

  void destroy(Program* p) {
      delete p;
  }
}
