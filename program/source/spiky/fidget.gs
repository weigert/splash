R""(

//Spiky Ball Fidget Spinner V1

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

uniform float vx; //Axis (1,0,0)
uniform float vy; //Axis (0,1,0)

void main() {

  //Expansion Value
  float k = 0.25; //Sensitivity
  float s = abs(vx)+abs(vy);
  s = k*s/(1+k*s); //Langmuir Style Attack

  //Max / Min Vals
  float _max = 5;
  float _min = 2;
  s = _min + s*(_max-_min);

  //Compute Centroid Position
  vec4 cpos = (gl_in[0].gl_Position+gl_in[1].gl_Position+gl_in[2].gl_Position);
  cpos.xyz = s*cpos.xyz / length(cpos);

  //Color of Centroid
  vec4 ccol = (exv[0].color + exv[1].color + exv[2].color)/3;

  //Construct Line Strip
  gl_Position = gl_in[0].gl_Position;
  exg.color = exv[0].color;
  EmitVertex();
  gl_Position = gl_in[1].gl_Position;
  exg.color = exv[1].color;
  EmitVertex();
  gl_Position = gl_in[2].gl_Position;
  exg.color = exv[2].color;
  EmitVertex();
  gl_Position = gl_in[0].gl_Position;
  exg.color = exv[0].color;
  EmitVertex();
  gl_Position = cpos;
  exg.color = ccol;
  EmitVertex();
  gl_Position = gl_in[1].gl_Position;
  exg.color = exv[1].color;
  EmitVertex();
  gl_Position = gl_in[2].gl_Position;
  exg.color = exv[2].color;
  EmitVertex();
  gl_Position = cpos;
  exg.color = ccol;
  EmitVertex();
  EndPrimitive();

}

)""
