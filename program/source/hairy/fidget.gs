R""(

//Hairy Ball Fidget Spinner V1
#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 8) out;

in vdata{
vec4 color;
flat vec3 normal;
flat vec3 fragpos;
} exv[];

out gdata{
vec4 color;
} exg;

//Parameters
float r = 0.025;
int NSEG = 8;
float sensitivity = 0.2;

uniform float vx; //Axis (1,0,0)
uniform float vy; //Axis (0,1,0)

vec3 lpos = vec3(5,5,5);

void emitHair(int ind){

  vec3 n = normalize(exv[ind].normal);

  vec3 n0 = n;

  vec3 p = gl_in[ind].gl_Position.xyz;

  float shade = clamp(dot(n0, lpos), 0.3, 0.9);
  exg.color = vec4(vec3(1.0), 2.0)-exv[ind].color;//vec4(vec3(shade), 1.0);

  //Starting Position
  gl_Position = vec4(p, 1.0);
  EmitVertex();

  //Add Positions
  for(int i = 0; i < NSEG; i++){

    //Compute Position
    p += r*n;
    gl_Position = vec4(p, 1.0);

    EmitVertex();

    //Update Direction for Next Segment
    vec3 r = vx*vec3(1,0,0)+vy*vec3(0,1,0);   //Rotation Vector
    n = normalize(mix(n, cross(n, r), sensitivity));  //Cross Product
  }

  EndPrimitive();
}

void main() {
  emitHair(0);
  emitHair(1);
  emitHair(2);
}

)""
