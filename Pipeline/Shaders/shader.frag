#version 450

layout(location = 0) in vec3 fragColors;
layout(location = 0) out vec4 out_Colors;

void main(){
	out_Colors = vec4(fragColors, 1.0f);
}
	