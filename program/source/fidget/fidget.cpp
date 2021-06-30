#include "../../program.h"

class Fidget : public Program {
private:

  //Shader Source
  const std::string vs_source =
    #include "fidget.vs"
  ;
  const std::string fs_source =
    #include "fidget.fs"
  ;

  //Utility Classes
  Model mesh;
  Shader* shader;

  //Dynamics Properties
  float acc = 0.1;    //Acceleration
  float frc = 0.01;   //Friction

public:

  int WIDTH, HEIGHT;
  float zoom = 0.05;
  glm::mat4 camera = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0,1,0));
  glm::mat4 projection;

  string test = "Worked!";

  float rot[3] = {0.0f};
	float vrot[3] = {0.0f}; //Initial Velocity
	float trot[3] = {0.0f}; //Target Velocity ()

  Fidget(svec* s, parse::data* d):Program(s){
    WIDTH = d->w;
    HEIGHT = d->h;

    projection = glm::ortho(-(float)WIDTH*zoom, (float)WIDTH*zoom, -(float)HEIGHT*zoom, (float)HEIGHT*zoom, -50.0f, 50.0f);

    //Setup Stuff
    shader = new Shader({vs_source, fs_source}, {"in_Position", "in_Normal"}, true);
    mesh.construct(_build);
  }

  virtual void pipeline(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();														//Prepare Shader
    shader->uniform("model", mesh.model);			//Set Model Matrix
    shader->uniform("vp", projection*camera);	//View Projection Matrix

    mesh.render(GL_LINES);									  //Render Model with Lines
  }

  virtual void event(event::eventdata& e){
    glm::mat4 R1 = glm::rotate(glm::mat4(1), glm::radians(vrot[0]), glm::vec3(1, 0, 0));
    glm::mat4 R2 = glm::rotate(glm::mat4(1), glm::radians(vrot[1]), glm::vec3(0, 1, 0));
    mesh.model = R1*R2*mesh.model;

    if(e.clicked){
      vrot[1] += acc*e.rx;
      vrot[0] += acc*e.ry;
    }

    vrot[0] += frc*(trot[0]-vrot[0]);
    vrot[1] += frc*(trot[1]-vrot[1]);
    vrot[2] += frc*(trot[2]-vrot[2]);

    rot[0] += vrot[0];
    rot[1] += vrot[1];
    rot[2] += vrot[2];
  }

  virtual void onpipe(std::string s){
    /* ... */
  }

  ~Fidget(){
      delete shader;
  }

  std::function<void(Model* m)> _build = [&](Model* h){

    //Radius r
    float r = 15.0f;

    //Icosahedron Vertices

    //Top Cap
    h->positions.push_back(0);
    h->positions.push_back(r);
    h->positions.push_back(0);

    float a = 72.0f/360.0f*2.0f*3.14159265f;

    for(int i = 0; i < 5; i++){ //Top
      float x1 = r*cos(atan(0.5))*cos(a*i);
      float y1 = r*cos(atan(0.5))*sin(a*i);
      float z1 = r*sin(atan(0.5));
      h->positions.push_back(x1);
      h->positions.push_back(z1);
      h->positions.push_back(y1);
    }

    for(int i = 0; i < 5; i++){ //Bottom
      float x2 = r*cos(atan(0.5))*cos(a*i+a/2.0f);
      float y2 = r*cos(atan(0.5))*sin(a*i+a/2.0f);
      float z2 = -r*sin(atan(0.5));
      h->positions.push_back(x2);
      h->positions.push_back(z2);
      h->positions.push_back(y2);
    }

    //Bottom Cap
    h->positions.push_back(0);
    h->positions.push_back(-r);
    h->positions.push_back(0);

    for(int i = 0; i < 12; i++){
      h->normals.push_back(1.0);
      h->normals.push_back(1.0);
      h->normals.push_back(1.0);
    }

    h->indices.push_back(1);
    h->indices.push_back(2);

    h->indices.push_back(2);
    h->indices.push_back(3);

    h->indices.push_back(3);
    h->indices.push_back(4);

    h->indices.push_back(4);
    h->indices.push_back(5);

    h->indices.push_back(5);
    h->indices.push_back(1);

    h->indices.push_back(6);
    h->indices.push_back(7);

    h->indices.push_back(7);
    h->indices.push_back(8);

    h->indices.push_back(8);
    h->indices.push_back(9);

    h->indices.push_back(9);
    h->indices.push_back(10);

    h->indices.push_back(10);
    h->indices.push_back(6);

    for(int i = 1; i < 6; i++){
      h->indices.push_back(i);
      h->indices.push_back(i+5);
    }

    h->indices.push_back(1);
    h->indices.push_back(10);

    h->indices.push_back(2);
    h->indices.push_back(6);

    h->indices.push_back(3);
    h->indices.push_back(7);

    h->indices.push_back(4);
    h->indices.push_back(8);

    h->indices.push_back(5);
    h->indices.push_back(9);

    //Caps
    for(int i = 1; i < 6; i++){
      h->indices.push_back(0);
      h->indices.push_back(i);

      h->indices.push_back(11);
      h->indices.push_back(11-i);
    }
  };
};

//Exporter
extern "C" {
  Program* create(svec* s, parse::data* d) {
      return new Fidget(s, d);
  }

  void destroy(Program* p) {
      delete p;
  }
}
