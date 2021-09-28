#include "../../program.h"
#include <SDL2/SDL_ttf.h>
#include <unistd.h>

glm::vec4 hexparse(std::string s){

  //Remove Leading 0x
  if(s.substr(0,2) == "0x")
    s = s.substr(2);

  std::stringstream ss;
  int val = 0;
  glm::vec4 vec(0);

  for(int i = 0; i < 4; i++){
    if(s.empty()) break;
    ss.clear();
    ss << std::hex << s.substr(0,2);
    ss >> val;
    vec[i] = val;
    s = s.substr(2);
  }

  return vec;
}

class Text : public Program {
private:

  //Utility Classes
  Square2D flat;
  Shader* shader = NULL;
  Texture* tex = NULL;

  int fontsize = 18;
  string fontface = ".ttf";
  int style;

  SDL_Color colorf;
  glm::vec4 color = glm::vec4(0,0,0,255);
  TTF_Font* font = NULL;
  glm::vec4 bgcolor = glm::vec4(0,0,0,0);

  int halign = 0; //0 = center, 1 = left, 2 = right
  int valign = 0; //0 = center, 1 = up, 2 = down
  int W, H;

  glm::mat4 model = glm::mat4(1.0);

  //Shader Sources
  const std::string vs_source =
    #include "text.vs"
  ;
  const std::string fs_source =
    #include "text.fs"
  ;

  void settext(std::string line){

    if(tex != NULL)
      delete tex;

    tex = new Texture();

    //Construct a surface from the text
    SDL_Surface* surface = TTF_RenderText_Blended(font, line.c_str(), colorf);

    tex->raw(surface, [](Texture* t){ //Setup Tex Parameters Correctly
      glTexParameteri(t->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(t->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(t->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(t->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(t->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    });

    //Scale the text to font size
    glm::vec3 scale = glm::vec3((float)tex->w/(float)W, (float)tex->h/(float)H, 1.0);
    glm::vec3 shift = glm::vec3(0); //Default: Centered

    if(halign == 1)       //Left
      shift.x -= ((float)W - (float)tex->w)/(float)W;
    else if(halign == 2)  //Right
      shift.x += ((float)W - (float)tex->h)/(float)W;

    if(valign == 1)       //Up
      shift.y += ((float)H - (float)tex->h)/(float)H;
    else if(valign == 2)  //Down
      shift.y -= ((float)H - (float)tex->h)/(float)H;

    model = glm::mat4(1);
    model = glm::translate(model, shift);
    model = glm::scale(model, scale);

  }

public:

  //Constructor
  Text(svec* s, parse::data* d):Program(s){

    TTF_Init();

    //Font Size / Face
    if(d->pflags["-fs"]) fontsize = stoi(d->params["-fs0"]);
    if(d->pflags["-ff"]) fontface = d->params["-ff0"];
    else{
      std::cout<<"No Font Specified"<<std::endl;
      alive = false;
      return;
    }

    //Open Font
    const char * home = getenv ("HOME");
    if(home == NULL){
      std::cout<<"Home Environment variable not set!"<<std::endl;
      alive = false;
      return;
    }
    fs::path fp = fs::path(home);

    font = TTF_OpenFont((fp/".fonts"/fontface).string().c_str(), fontsize);
    if(!font){
      std::cout<<"Couldn't open font! Not Found: "<<(fp/".fonts"/fontface).string()<<std::endl;
      alive = false;
      return;
    }

    //Style Parameters
    style = TTF_STYLE_NORMAL;
    if(d->flags["--fu"]) style |= TTF_STYLE_UNDERLINE;
    if(d->flags["--fs"]) style |= TTF_STYLE_STRIKETHROUGH;
    if(d->flags["--fi"]) style |= TTF_STYLE_ITALIC;
    if(d->flags["--fb"]) style |= TTF_STYLE_BOLD;

    TTF_SetFontStyle(font, style);

    //Extract Color
    if(d->pflags["-fc"])
      color = hexparse(d->params["-fc0"]);
    colorf = {color.x, color.y, color.z, color.w};

    if(d->pflags["-bc"])
      bgcolor = hexparse(d->params["-bc0"])/255.0f;

    //Vertical / Horizontal Alignment
    if(d->pflags["-v"]) valign = stoi(d->params["-v0"]);
    if(d->pflags["-h"]) halign = stoi(d->params["-h0"]);
    W = d->w; H = d->h;

    //Create Shader
    shader = new Shader({vs_source, fs_source}, {"in_Quad, in_Tex"}, true);

    //Create Text Texture
    if(!s->empty())
      settext(s->back());

  }

  virtual void pipeline(){
    if(tex == NULL) return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader->use();
    shader->uniform("model", model);
    shader->uniform("bgcolor", bgcolor);
    shader->texture("imageTexture", *tex);
    flat.render();
  }

  virtual void event(event::eventdata& e){
    /* ... */
  }

  virtual void onpipe(std::string s){
    settext(s);
  }

  ~Text(){
    if(shader != NULL)  delete shader;
    if(tex != NULL)     delete tex;
    if(font != NULL)    TTF_CloseFont(font);
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
