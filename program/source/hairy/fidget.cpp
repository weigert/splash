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
  const std::string gs_source =
    #include "fidget.gs"
  ;
  const std::string vs_solid =
    #include "shader/solid.vs"
  ;
  const std::string fs_solid =
    #include "shader/solid.fs"
  ;

  //Utility Classes
  Model mesh;
  Shader* effect;
  Shader* solid;

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
    effect = new Shader({vs_source, gs_source, fs_source}, {"in_Position", "in_Normal"}, true);
    solid = new Shader({vs_solid, fs_solid}, {"in_Position", "in_Normal"}, true);

    mesh.construct(_build);

    //Split and Update
    for(int i = 0; i < 3; i++)
      split(&mesh);
    mesh.update();
  }

  virtual void pipeline(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    solid->use();														 //Prepare Shader
    solid->uniform("model", mesh.model);			//Set Model Matrix
    solid->uniform("vp", projection*camera);	//View Projection Matrix

    mesh.render(GL_TRIANGLES);							  //Render Model with Lines

    //Add Hair Effect
    effect->use();														//Prepare Shader
    effect->uniform("model", mesh.model);			//Set Model Matrix
    effect->uniform("vp", projection*camera);	//View Projection Matrix

    //Information about the Rotation
    effect->uniform("vx", vrot[0]);
    effect->uniform("vy", vrot[1]);

    mesh.render(GL_TRIANGLES);							  //Render Model with Lines


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

  ~Fidget(){
    delete effect;
    delete solid;
  }

  std::function<void(Model* m)> _build = [&](Model* h){

    //Radius r
    float r = 10.0f;
    float a = 72.0f/360.0f*2.0f*3.14159265f;

    //Helper Methods
    auto addpos = [&](float a, float b, float c){
      h->positions.push_back(a);
      h->positions.push_back(b);
      h->positions.push_back(c);
    };

    //For a Line-Strip
    /*
    auto triangle = [&](int a, int b, int c){
      h->indices.push_back(a);
      h->indices.push_back(b);
      h->indices.push_back(b);
      h->indices.push_back(c);
      h->indices.push_back(c);
      h->indices.push_back(a);
    };
    */

    //For a Triangles
    auto triangle = [&](int a, int b, int c){
      h->indices.push_back(a);
      h->indices.push_back(b);
      h->indices.push_back(c);
    };

    //Top Cap
    addpos(0, r, 0);

    for(int i = 0; i < 5; i++){ //Top
      float x1 = r*cos(atan(0.5))*cos(a*i);
      float y1 = r*cos(atan(0.5))*sin(a*i);
      float z1 = r*sin(atan(0.5));
      addpos(x1,z1,y1);
    }

    for(int i = 0; i < 5; i++){ //Bottom
      float x2 = r*cos(atan(0.5))*cos(a*i+a/2.0f);
      float y2 = r*cos(atan(0.5))*sin(a*i+a/2.0f);
      float z2 = -r*sin(atan(0.5));
      addpos(x2,z2,y2);
    }

    //Bottom Cap
    addpos(0,-r,0);

    for(int i = 0; i < 12; i++){
      glm::vec3 n = glm::vec3(h->positions[3*i+0],h->positions[3*i+1],h->positions[3*i+2]);
      n = glm::normalize(n);
      h->normals.push_back(n.x);
      h->normals.push_back(n.y);
      h->normals.push_back(n.z);
    }

    //Top Triangles
    triangle(0,1,2);
    triangle(0,2,3);
    triangle(0,3,4);
    triangle(0,4,5);
    triangle(0,5,1);

    //Bottom Triangles
    triangle(6,7,11);
    triangle(7,8,11);
    triangle(8,9,11);
    triangle(9,10,11);
    triangle(10,6,11);

    //Connecting Triangles (Bottom)
    triangle(6,7,2);
    triangle(7,8,3);
    triangle(8,9,4);
    triangle(9,10,5);
    triangle(10,6,1);

    //Connecting Triangles (Top)
    triangle(2,3,7);
    triangle(1,2,6);
    triangle(3,4,8);
    triangle(4,5,9);
    triangle(5,1,10);

  };


  std::function<void(Model* m)> split = [&](Model* h){

    std::vector<GLuint> newind;

    auto addpos = [&](glm::vec3 p){
      h->positions.push_back(p.x);
      h->positions.push_back(p.y);
      h->positions.push_back(p.z);
    };

    auto addnorm = [&](glm::vec3 p){
      glm::vec3 n = glm::normalize(p);
      h->normals.push_back(n.x);
      h->normals.push_back(n.y);
      h->normals.push_back(n.z);
    };

    auto triangle = [&](int a, int b, int c){
      newind.push_back(a);
      newind.push_back(b);
      newind.push_back(c);
    };

    float r = 10.0f;

    for(int i = 0; i < h->indices.size()/3; i++){   //Loop over Old Triangles

      //Indicies of the old triangle
      GLuint k1 = h->indices[3*i+0];
      GLuint k2 = h->indices[3*i+1];
      GLuint k3 = h->indices[3*i+2];

      //Positions of the old triangle
      glm::vec3 a = glm::vec3(h->positions[3*k1+0], h->positions[3*k1+1], h->positions[3*k1+2]);
      glm::vec3 b = glm::vec3(h->positions[3*k2+0], h->positions[3*k2+1], h->positions[3*k2+2]);
      glm::vec3 c = glm::vec3(h->positions[3*k3+0], h->positions[3*k3+1], h->positions[3*k3+2]);

      //Compute Split Points
      glm::vec3 d = 0.5f*(a+b);
      glm::vec3 e = 0.5f*(b+c);
      glm::vec3 f = 0.5f*(c+a);

      //Normalize vectors to project onto sphere
      d = r*glm::normalize(d);
      e = r*glm::normalize(e);
      f = r*glm::normalize(f);

      //Starting Indices
      int NI = h->positions.size()/3;

      //Add New Positions to Model
      addpos(d); addpos(e); addpos(f);
      addnorm(d); addnorm(e); addnorm(f);

      triangle(k1, NI+0, NI+2);
      triangle(k2, NI+1, NI+0);
      triangle(k3, NI+2, NI+1);
      triangle(NI+0, NI+1, NI+2);
    }

    //Copy Indices
    h->indices = newind;
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
