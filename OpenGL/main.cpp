#include <stdio.h>
#include <stdlib.h>
#include <optional>
#include <functional>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "loadShader.h"

using namespace std;


static const GLfloat g_vertex_buffer_data[] = {
	0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
};

optional<GLFWwindow*> Setup() {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return nullopt;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // On veut OpenGL 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL

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

int main(int argc, char* argv[]) {

	// Setup

	GLFWwindow* window;
	if (auto windowOpt = Setup()) {
		window = windowOpt.value();
	}
	else {
		return -1;
	}

	// Object for drawing
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint programID = LoadShaders("../shaders/TestVertexShader.shader", "../shaders/TestFragmentShader.shader");

	double t;

	GLuint scale_uniform_location = glGetUniformLocation(programID, "scale");
	GLuint scale_frag_uniform_location = glGetUniformLocation(programID, "scale_frag");
	GLuint width_uniform_location = glGetUniformLocation(programID, "width");
	GLuint height_uniform_location = glGetUniformLocation(programID, "height");

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		t = glfwGetTime();
		float scale_uniform = std::sin(t);
		glUniform1f(scale_uniform_location, scale_uniform);
		glUniform1f(scale_frag_uniform_location, scale_uniform);
		glUniform1i(width_uniform_location, w);
		glUniform1i(height_uniform_location, h);


		//Loop body // Draw here
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
			0,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			0,					// stride
			(void*)0			// array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	return 0;
}