#version 450 core

in vec3 color_out;
in vec2 uv_out;
in vec3 normal_out;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D colorTexture;

void main()
{
	color = vec4(0,0,0,1);
}