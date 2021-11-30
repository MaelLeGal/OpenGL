#include <stdio.h>
#include <stdlib.h>
#include <optional>
#include <functional>
#include <iostream>
#include <cstddef>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "loadShader.h"
#include "stb_image.h";

using namespace std;

struct vertex_struct {

	GLfloat vertexbuffer[3];
	GLfloat color[3];
	GLfloat uv[2];
	GLfloat normal[3];
	GLfloat texCoord[2];
};

static const std::vector<vertex_struct> vertices{
	{{0.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f,-1.0f},{0.0f,0.0f}},
	{{1.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f}, {1.0f,0.0f}, {0.0f,0.0f,-1.0f},{1.0f,0.0f}},
	{{0.0f,1.0f,0.0f}, {0.0f,0.0f,1.0f}, {0.0f,1.0f}, {0.0f,0.0f,-1.0f},{0.0f,1.0f}}
};

static const GLfloat g_vertex_buffer_data[] = {
	0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
};

static void error_callback(int /*error*/, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

void APIENTRY opengl_error_callback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::cout << message << std::endl;
}

optional<GLFWwindow*> Setup() {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return nullopt;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // On veut OpenGL 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "Tutorial", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		return nullopt;
	}
	glfwMakeContextCurrent(window); // Initialise GLEW
	glewExperimental = true; // Nécessaire dans le profil de base

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return nullopt;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	return window;
}

/*template<typename Type> void EnableAttrib(char* name, GLuint programID, int size) {
	GLint location = glGetAttribLocation(programID, name);
	glVertexAttribPointer(
		location,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
		size,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		sizeof(Type),					// stride
		(void*)offsetof(Type, Type::vertexbuffer)			// array buffer offset
	);
	glEnableVertexAttribArray(location);
}*/

int main(int argc, char* argv[]) {

	// Setup

	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	if (auto windowOpt = Setup()) {
		window = windowOpt.value();
	}
	else {
		return -1;
	}
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(opengl_error_callback, nullptr);

	// Object for drawing
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_struct)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

	GLuint programID = LoadShaders("../shaders/TestVertexShader.shader", "../shaders/TestFragmentShader.shader");

	double t;

	GLuint scale_uniform_location = glGetUniformLocation(programID, "scale");
	GLuint scale_frag_uniform_location = glGetUniformLocation(programID, "scale_frag");
	GLuint width_uniform_location = glGetUniformLocation(programID, "width");
	GLuint height_uniform_location = glGetUniformLocation(programID, "height");
	GLuint mode_location = glGetUniformLocation(programID, "mode");

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	int mode = 0;

	glUseProgram(programID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	GLuint textures;
	if (data) {
		glCreateTextures(GL_TEXTURE_2D, 1, &textures);
		glTextureStorage2D(textures, 1, GL_RGB8, width, height);
		glTextureSubImage2D(textures, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

		glBindTextureUnit(0, textures);
		GLuint texture_color_location = glGetUniformLocation(programID, "colorTexture");
		glUniform1i(texture_color_location, 0);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	

	stbi_image_free(data);

	// Start loop
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		t = glfwGetTime();
		float scale_uniform = std::sin(t);

		if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			mode = 0;
		}
		else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			mode = 1;
		}
		else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			mode = 2;
		}
		else {
			mode = -1;
		}

		glUniform1f(scale_uniform_location, scale_uniform);
		glUniform1f(scale_frag_uniform_location, scale_uniform);
		glUniform1i(width_uniform_location, w);
		glUniform1i(height_uniform_location, h);
		glUniform1i(mode_location, mode);


		//Loop body // Draw here
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		GLint positionLocation = glGetAttribLocation(programID, "vertexPosition_modelspace");
		glVertexAttribPointer(
			positionLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::vertexbuffer)			// array buffer offset
		);
		glEnableVertexAttribArray(positionLocation);

		//EnableAttrib<vertex_struct>("vertexPosition_modelspace", programID, 3);

		GLint colorLocation = glGetAttribLocation(programID, "color");
		glVertexAttribPointer(
			colorLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::color)			// array buffer offset
		);
		glEnableVertexAttribArray(colorLocation);

		GLint uvLocation = glGetAttribLocation(programID, "uv");
		glVertexAttribPointer(
			uvLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::uv)			// array buffer offset
		);
		glEnableVertexAttribArray(uvLocation);

		GLint normalLocation = glGetAttribLocation(programID, "normal");
		glVertexAttribPointer(
			normalLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::normal)			// array buffer offset
		);
		glEnableVertexAttribArray(normalLocation);

		GLint textureCoordinatesLocation = glGetAttribLocation(programID, "aTexCoord");
		glVertexAttribPointer(
			textureCoordinatesLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::texCoord)			// array buffer offset
		);
		glEnableVertexAttribArray(textureCoordinatesLocation);

		glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	return 0;
}