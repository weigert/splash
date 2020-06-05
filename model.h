#include "noise/noise.h"

//Model Stuff
int SEED = 10;
double scale = 30.0;
double heightmap[64][64] = {0.0};
glm::vec2 dim = glm::vec2(64);
noise::module::Perlin perlin;

//View Stuff
const int WIDTH = 1920;
const int HEIGHT = 1200;
glm::vec3 viewPos = glm::vec3(50, 1, 50);
float zoom = 0.05;
float zoomInc = 0.001;
float rotation = 0.0f;
glm::vec3 cameraPos = glm::vec3(50, 50, 50);
glm::vec3 lookPos = glm::vec3(0, 0, 0);
glm::mat4 camera = glm::lookAt(cameraPos, lookPos, glm::vec3(0,1,0));
glm::mat4 projection;

void setup(){
  //Projection Matrix (Orthographic)
  projection = glm::ortho(-(float)WIDTH*zoom, (float)WIDTH*zoom, -(float)HEIGHT*zoom, (float)HEIGHT*zoom, -800.0f, 500.0f);

  perlin.SetOctaveCount(8);
  perlin.SetFrequency(1.0);
  perlin.SetPersistence(0.6);

  float min, max = 0.0;
  for(int i = 0; i < dim.x; i++){
    for(int j = 0; j < dim.y; j++){
      heightmap[i][j] = perlin.GetValue(i*(1.0/dim.x), j*(1.0/dim.y), SEED);
      if(heightmap[i][j] > max) max = heightmap[i][j];
      if(heightmap[i][j] < min) min = heightmap[i][j];
    }
  }

  //Normalize
  for(int i = 0; i < dim.x; i++){
    for(int j = 0; j < dim.y; j++){
      //Normalize to (0, 1) scale.
      heightmap[i][j] = (heightmap[i][j] - min)/(max - min);
    }
  }
};

// Model Constructing Function
std::function<void(Model* m)> _construct = [&](Model* h){

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
    h->normals.push_back(0.0);
    h->normals.push_back(0.0);
    h->normals.push_back(0.0);
  }

/*
  //Top and Bottom
  for(int i = 0; i < 4; i++){
    int j = i*2+1;
    int k = (i*2+3)%12;

    h->indices.push_back(0);
    h->indices.push_back(j);
    h->indices.push_back(0);
    h->indices.push_back(k);
    h->indices.push_back(j);
    h->indices.push_back(k);
  }
*/

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
