#version 450 core

layout(location = 0) in vec3 vertexPosition_modelspace;
in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 modelPlayer;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj*view* modelPlayer *vec4(vertexPosition_modelspace,1);
	TexCoord = aTexCoord;
}