R""(

#version 330 core

in gdata{
vec4 color;
} exg;

out vec4 fragColor;

void main(void) {
  fragColor = exg.color;
}

)""
