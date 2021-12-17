#version 450 core

in vec3 color_out;
in vec2 uv_out;
in vec3 normal_out;
in vec2 TexCoord;

out vec4 color;

uniform float scale_frag;
uniform int width;
uniform int height;
uniform int mode;
uniform sampler2D colorTexture;

void main()
{
	//color = vec4(abs(scale_frag)*(gl_FragCoord.x/width), abs(scale_frag)*(gl_FragCoord.y/height), abs(scale_frag)*1,1);
	if (mode == 0) {
		color = vec4(color_out, 1);
	}
	else if (mode == 1) {
		color = vec4(normal_out, 1);
	}
	else if (mode == 2) {
		color = vec4(uv_out, 1, 1);
	}
	else {
		//color = vec4(abs(scale_frag) * (gl_FragCoord.x / width), abs(scale_frag) * (gl_FragCoord.y / height), abs(scale_frag) * 1, 1);
		//color = texture(colorTexture, TexCoord);
		color = vec4(0,0,0,1);
	}
}