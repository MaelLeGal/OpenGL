#version 450 core

layout(location = 0) in vec3 vertexPosition_modelspace;
in vec3 color;
in vec2 uv;
in vec3 normal;
in vec2 aGroundTexCoord;

out vec3 color_out;
out vec2 uv_out;
out vec3 normal_out;
out vec2 TexCoord;

void main()
{
	gl_Position = vec4(vertexPosition_modelspace,1);
	color_out = color;
	uv_out = uv;
	normal_out = normal;
	TexCoord = aGroundTexCoord;
}