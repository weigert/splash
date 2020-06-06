class Fidget : public Program {
public:

  int WIDTH, HEIGHT;
  glm::vec3 viewPos = glm::vec3(50, 1, 50);
  float zoom = 0.05;
  float zoomInc = 0.001;
  float rotation = 0.0f;
  glm::vec3 cameraPos = glm::vec3(50, 50, 50);
  glm::mat4 camera = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), glm::vec3(0,1,0));
  glm::mat4 projection;

  float rot[3] = {0.0f};
	float vrot[3] = {1.0f}; //Initial Velocity
	float trot[3] = {0.5f}; //Target Velocity ()

  Fidget(int w, int h){
    WIDTH = w;
    HEIGHT = h;

    projection = glm::ortho(-(float)WIDTH*zoom, (float)WIDTH*zoom, -(float)HEIGHT*zoom, (float)HEIGHT*zoom, -800.0f, 500.0f);

    //Setup Stuff
    shader = new Shader({"program/fidget/fidget.vs", "program/fidget/fidget.fs"}, {"in_Position", "in_Normal"});
    mesh.construct(_build);

    pipeline = [&](){

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      shader->use();														//Prepare Shader
  		shader->uniform("model", mesh.model);			//Set Model Matrix
  		shader->uniform("vp", projection*camera);	//View Projection Matrix
  		mesh.render(GL_LINES);													//Render Model with Lines

    };

    event = [&](){

  		//Prepare Camera
  		camera = glm::rotate(glm::mat4(1.0), glm::radians(rot[0]), glm::vec3(1, 0, 0));
  		glm::vec4 newup = glm::inverse(camera)*glm::vec4(0.0f, 1.0f, 0.0f, 1.0);
  		camera = glm::rotate(camera, glm::radians(rot[1]), glm::vec3(newup.x, newup.y, newup.z));

  		if(event::clicked){
  			//Accelerate
  			vrot[1] += 0.05f*event::rx;
  			vrot[0] += 0.05f*event::ry;
  		}

  		//Approach Goal Velocity
  		vrot[0] += 0.05*(trot[0]-vrot[0]);		//Friction
  		vrot[1] += 0.05*(trot[1]-vrot[1]);
  		vrot[2] += 0.05*(trot[2]-vrot[2]);

  		rot[0] += vrot[0];	//Speed
  		rot[1] += vrot[1];
  		rot[2] += vrot[2];

  	};


  }

  Model mesh;
	Shader* shader;

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
      h->normals.push_back(0.0);
      h->normals.push_back(0.0);
      h->normals.push_back(0.0);
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
