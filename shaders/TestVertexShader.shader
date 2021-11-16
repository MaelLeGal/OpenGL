#version 450 core
layout(location = 0) in vec3 vertexPosition_modelspace;

uniform float scale;

void main()
{
	gl_Position = vec4(vertexPosition_modelspace*scale,1);
}