R""(

#version 330 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

uniform mat4 model;
uniform mat4 vp;

out vdata{
vec4 color;
flat vec3 normal;
flat vec3 fragpos;
} exv;

void main(void) {
	exv.fragpos = 0.85*(model * vec4(in_Position, 1.0f)).xyz;
	exv.normal = (model * vec4(in_Normal, 1.0f)).xyz;
	gl_Position = vp * vec4(exv.fragpos, 1.0f);
	exv.color = vec4(in_Normal, 1.0);
}

)""
