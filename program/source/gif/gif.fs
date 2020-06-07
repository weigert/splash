R""(

#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

void main(){
  fragColor = texture(imageTexture, ex_Tex);
  if(fragColor.a == 0.0) discard;

  //Invert Color
  //fragColor = vec4(vec3(1.0) - fragColor.xyz, 1.0);
}

)""
