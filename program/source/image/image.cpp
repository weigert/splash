class Image : public Program {
public:
  Image(svec* s):Program(s){

    shader = new Shader({"program/image/image.vs", "program/image/image.fs"}, {"in_Quad, in_Tex"});
    tex = new Texture(image::load(data->back()));

    pipeline = [&](){

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      shader->use();
      shader->uniform("model", flat.model);
      shader->texture("imageTexture", *tex);
      flat.render();

    };

  }

  Square2D flat;
  Shader* shader;
  Texture* tex;

  ~Image(){
      delete shader;
      delete tex;
  }

};
