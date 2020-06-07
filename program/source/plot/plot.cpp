#include <sstream>

class Plot : public Program {
private:
  //Utility Classes
  Shader* shader;
  Model mesh;
  Square2D flat;

  //Mesh Constructor
  std::function<void(Model*, svec*)> build;

  glm::mat4 vp = glm::mat4(1);

public:

  Plot(svec* s):Program(s){


    build = [&](Model* h, svec* s){
      string t;
      while(!s->empty()){

        t = s->back();
        s->pop_back();

        stringstream ss;
        ss << t;

        //Extract Numbers
        std::vector<float> f;
        string temp;
        float found;
        while (!ss.eof()) {
          ss >> temp;
          if (stringstream(temp) >> found)
            f.push_back(found);
          //  cout << found << std::endl;
          temp = "";
        }

        //Construct Plot
        int N = f.size();
        std::cout<<N<<std::endl;
        for(int i = 0; i < N; i++){
          h->positions.push_back(i/(float)N);
          h->positions.push_back(f[i]);
          h->positions.push_back(0);

          h->normals.push_back(1);
          h->normals.push_back(1);
          h->normals.push_back(1);
        }

        for(int i = 1; i < N; i++){
          h->indices.push_back(i-1);
          h->indices.push_back(i);
        }
      }
    };

    shader = new Shader({"program/plot/plot.vs", "program/plot/plot.fs"},
                        {"in_Position, in_Normal"});

    mesh.construct(build, s);

    pipeline = [&](){
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      shader->use();
      shader->uniform("vp", vp);
      shader->uniform("model", mesh.model);
      mesh.render(GL_LINES);

    };

  }

  ~Plot(){
      delete shader;
  }

};
