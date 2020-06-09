R""(

#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

void main(){
  fragColor = texture(imageTexture, ex_Tex);
  if(fragColor.a == 0.0) discard;//fragColor = vec4(0.0, 0.0, 0.0, 0.5);//discard;
}

)""
