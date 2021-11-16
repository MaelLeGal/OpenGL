#version 450 core
out vec3 color;

uniform float scale_frag;
uniform int width;
uniform int height;

void main()
{
	color = vec3(abs(scale_frag)*(gl_FragCoord.x/width), abs(scale_frag)*(gl_FragCoord.y/height), abs(scale_frag)*1);
}