class Shader{
public:
  Shader(slist shaders, slist in){
    program = glCreateProgram();        //Generate Shader
    setup(shaders);                     //Add Individual Shaders
    for(auto &n : in)                   //Add all Attributes of Shader
      glBindAttribLocation(program, &n - in.begin(), n.c_str());
    link();                             //Link the shader program!
  }

  Shader(slist shaders, slist in, bool source){
    program = glCreateProgram();        //Generate Shader

    std::vector<std::string> _src = shaders;

    char* src;
    int32_t size;

    if(shaders.size() == 2){

      vertexShader = glCreateShader(GL_VERTEX_SHADER);
      src = const_cast<char*>(_src[0].c_str());
      size = _src[0].length();
      glShaderSource(vertexShader, 1, &src, &size);
      compile(vertexShader);

      fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
      src = const_cast<char*>(_src[1].c_str());
      size = _src[1].length();
      glShaderSource(fragmentShader, 1, &src, &size);
      compile(fragmentShader);

    }
    else if(shaders.size() == 3){

      vertexShader = glCreateShader(GL_VERTEX_SHADER);
      src = const_cast<char*>(_src[0].c_str());
      size = _src[0].length();
      glShaderSource(vertexShader, 1, &src, &size);
      compile(vertexShader);

      geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
      src = const_cast<char*>(_src[1].c_str());
      size = _src[1].length();
      glShaderSource(geometryShader, 1, &src, &size);
      compile(geometryShader);

      fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
      src = const_cast<char*>(_src[2].c_str());
      size = _src[2].length();
      glShaderSource(fragmentShader, 1, &src, &size);
      compile(fragmentShader);

    }
    else std::cout<<"Number of Shaders not Recognized"<<std::endl;

    for(auto &n : in)                   //Add all Attributes of Shader
      glBindAttribLocation(program, &n - in.begin(), n.c_str());
    link();                             //Link the shader program!
  }

  ~Shader(){
    glDeleteProgram(program);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
    glDeleteShader(vertexShader);
  }

  GLuint program;   //Shader Program ID
  GLuint vertexShader, geometryShader, fragmentShader;
  int boundtextures;

  void setup(slist shaders);
  int  addProgram(std::string fileName, GLenum shaderType);  //General Shader Addition
  std::string readGLSLFile(std::string fileName, int32_t &size); //Read File
  bool compile(GLuint shader);  //Compile and Add File
  bool link();                  //Link the entire program
  void error(GLuint s, bool t); //Get Compile/Link Error
  void use();                   //Use the program

  template<typename T> void texture(std::string name, const T& t);
  template<typename T> void uniform(std::string name, const T u);
  template<typename T, size_t N> void uniform(std::string name, const T (&u)[N]);
};

void Shader::setup(slist _s){
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  std::vector<std::string> s = _s;

  if(s.size() == 2){
    vertexShader   = addProgram((data_dir/s[0]).string(), GL_VERTEX_SHADER);
    fragmentShader = addProgram((data_dir/s[1]).string(), GL_FRAGMENT_SHADER);
  }
  else if(s.size() == 3){
    vertexShader   = addProgram((data_dir/s[0]).string(), GL_VERTEX_SHADER);
    geometryShader = addProgram((data_dir/s[1]).string(), GL_GEOMETRY_SHADER);
    fragmentShader = addProgram((data_dir/s[2]).string(), GL_FRAGMENT_SHADER);
  }
  else std::cout<<"Number of shaders not recognized."<<std::endl;
}

int Shader::addProgram(std::string fileName, GLenum shaderType){
  char* src; int32_t size;
  std::string result = readGLSLFile(fileName, size);
  src = const_cast<char*>(result.c_str());

  int shaderID = glCreateShader(shaderType);
  glShaderSource(shaderID, 1, &src, &size);
  if(!compile(shaderID)){
    std::cout<<"Failed to compile "<<fileName<<std::endl;
  }

  return shaderID;
}

bool Shader::compile(GLuint shader){
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(success) glAttachShader(program, shader);
  else        error(shader, true);
//  std::cout<<"AYy"<<std::endl;
  return success;
}

bool Shader::link(){
  glLinkProgram(program);
  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if(!success) error(program, false);
  return success;
}

void Shader::error(GLuint s, bool t){
  int m;
  if(t) glGetShaderiv(s, GL_INFO_LOG_LENGTH, &m);
  else glGetProgramiv(s, GL_INFO_LOG_LENGTH, &m);
  char* l = new char[m];
  if(t) glGetShaderInfoLog(s, m, &m, l);
  else glGetProgramInfoLog(s, m, &m, l);
  std::cout<<"Linker Error: "<<l<<std::endl;
  delete[] l;
}

void Shader::use(){
  boundtextures = 0;
  glUseProgram(program);
}

std::string Shader::readGLSLFile(std::string file, int32_t &size){
  std::ifstream t;
  std::string fileContent;

  t.open(file);     //Read GLSL File Contents
  if(t.is_open()){
    std::stringstream buffer;
    buffer << t.rdbuf();
    fileContent = buffer.str();
  }
  else std::cout<<"File opening failed"<<std::endl;
  t.close();

  size = fileContent.length();  //Set the Size
  return fileContent;
}

/* Uniform Setters */

template<typename T>
void Shader::uniform(std::string name, T u){
  std::cout<<"Error: Data type not recognized for uniform "<<name<<"."<<std::endl; }

template<typename T, size_t N>
void Shader::uniform(std::string name, const T (&u)[N]){
  std::cout<<"Error: Data type not recognized for uniform "<<name<<"."<<std::endl; }

template<> void Shader::uniform(std::string name, const bool u){
  glUniform1i(glGetUniformLocation(program, name.c_str()), u); }

template<> void Shader::uniform(std::string name, const int u){
  glUniform1i(glGetUniformLocation(program, name.c_str()), u); }

template<> void Shader::uniform(std::string name, const float u){
  glUniform1f(glGetUniformLocation(program, name.c_str()), u); }

template<> void Shader::uniform(std::string name, const double u){ //GLSL Intrinsically Single Precision
  glUniform1f(glGetUniformLocation(program, name.c_str()), (float)u); }

template<> void Shader::uniform(std::string name, const glm::vec2 u){
  glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const glm::vec3 u){
  glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const float (&u)[3]){
  glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const float (&u)[4]){
  glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const glm::vec4 u){
  glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const glm::mat3 u){
  glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &u[0][0]); }

template<> void Shader::uniform(std::string name, const glm::mat4 u){
  glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &u[0][0]); }

template<> void Shader::uniform(std::string name, const std::vector<glm::mat4> u){
  glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), u.size(), GL_FALSE, &u[0][0][0]); }

template<typename T>
void Shader::texture(std::string name, const T& t){
  glActiveTexture(GL_TEXTURE0 + boundtextures);
  glBindTexture(t.type, t.texture);
  uniform(name, boundtextures++);
}
