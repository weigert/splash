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
  string fontface = ".ttf";

  int halign = 0; //0 = center, 1 = left, 2 = right
  int valign = 0; //0 = center, 1 = up, 2 = down

  glm::mat4 model = glm::mat4(1.0);

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

    if(d->pflags["-fs"])  //Font Size
      fontsize = stoi(d->params["-fs0"]);

    if(d->pflags["-ff"])
      fontface = d->params["-ff0"];
    else
      logger::fatal("No font specified");

    //Vertical Align
    if(d->flags["--vu"])
      valign = 1;
    else if(d->flags["--vc"])
      valign = 0;
    else if(d->flags["--vd"])
      valign = 2;

    //Horizontal Align
    if(d->flags["--hl"])
      halign = 1;
    else if(d->flags["--hc"])
      halign = 0;
    else if(d->flags["--hr"])
      halign = 2;

    unsigned int x = 0;
    std::stringstream ss;

    if(d->pflags["-fc"]){
      ss << std::hex << d->params["-fc0"];
      ss >> x;
    }
    glm::vec3 fc = glm::vec3(x%256, (x/(256))%256, (x/(256*256))%256);

    shader = new Shader({vs_source, fs_source}, {"in_Quad, in_Tex"}, true);

    if(s == NULL){
      logger::err("No Text Specified");
      exit(0);
    }

    int style = TTF_STYLE_NORMAL;
    if(d->flags["--fu"])
      style |= TTF_STYLE_UNDERLINE;
    if(d->flags["--fs"])
      style |= TTF_STYLE_STRIKETHROUGH;
    if(d->flags["--fi"])
      style |= TTF_STYLE_ITALIC;
    if(d->flags["--fb"])
      style |= TTF_STYLE_BOLD;

    const char * home = getenv ("HOME");
    if(home == NULL)
      logger::fatal("Home environment variable not set");
    fs::path fp = fs::path(home);

    TTF_Font* font = TTF_OpenFont((fp/".fonts"/fontface).string().c_str(), fontsize);
    if(!font) logger::fatal("Couldn't open font");

    TTF_SetFontStyle(font, style);

    SDL_Color colorf = {fc.x, fc.y, fc.z, 255 };
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
