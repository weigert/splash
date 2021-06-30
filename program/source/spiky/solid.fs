R""(

#version 330 core

out vec4 fragColor;

in vdata{
vec4 color;
flat vec3 normal;
flat vec3 fragpos;
} exv;

void main(void) {
  fragColor = vec4(vec3(0.0), 1.0);//exv.color;
}

)""
