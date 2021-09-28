R""(

#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;
uniform vec4 bgcolor;

void main(){

  fragColor = vec4(bgcolor);
  vec4 texcolor = texture(imageTexture, ex_Tex);
  fragColor = mix(fragColor, texcolor, texcolor.a);

}

)""
