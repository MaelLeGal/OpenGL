#include <stdio.h>
#include <stdlib.h>
#include <optional>
#include <functional>
#include <iostream>
#include <cstddef>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	{{0.0f,1.0f,0.0f}, {0.0f,0.0f,1.0f}, {0.0f,1.0f}, {0.0f,0.0f,-1.0f},{0.0f,1.0f}},
};

static const std::vector<vertex_struct> ground{
	{{-1.0f,-0.8f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f,-1.0f},{0.0f,0.0f}},
	{{-1.0f,-1.0f,0.0f}, {0.0f,1.0f,0.0f}, {1.0f,0.0f}, {0.0f,0.0f,-1.0f},{1.0f,0.0f}},
	{{1.0f,-0.8f,0.0f}, {0.0f,0.0f,1.0f}, {0.0f,1.0f}, {0.0f,0.0f,-1.0f},{0.0f,1.0f}},

	{{-1.0f,-1.0f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,1.0f}, {0.0f,0.0f,-1.0f},{0.0f,0.0f}},
	{{1.0f,-0.8f,0.0f}, {0.0f,1.0f,0.0f}, {1.0f,1.0f}, {0.0f,0.0f,-1.0f},{1.0f,0.0f}},
	{{1.0f,-1.0f,0.0f}, {0.0f,0.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f,-1.0f},{0.0f,1.0f}},
};

static const std::vector<vertex_struct> player{
	{{-0.2f,-0.8f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f,-1.0f},{0.0f,0.0f}},
	{{-0.2f,-0.4f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f,-1.0f},{0.0f,0.0f}},
	{{0.2f,-0.4f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f,-1.0f},{0.0f,0.0f}},

	{{0.2f,-0.8f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f,-1.0f},{0.0f,0.0f}},
	{{0.2f,-0.4f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f,-1.0f},{0.0f,0.0f}},
	{{-0.2f,-0.8f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f,-1.0f},{0.0f,0.0f}}
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
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_struct)*vertices.size(), vertices.data(), GL_STATIC_DRAW); //Buffer data plus grand avec bufferSubData, nullptr pour data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_struct) * (vertices.size() + ground.size() + player.size()), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_struct) * vertices.size(), vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_struct) * vertices.size(), sizeof(vertex_struct) * ground.size(), ground.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_struct) * (vertices.size() + ground.size()), sizeof(vertex_struct) * player.size(), player.data());


	GLuint programID = LoadShaders("../shaders/TestVertexShader.shader", "../shaders/TestFragmentShader.shader");
	GLuint groundProgramID = LoadShaders("../shaders/GroundVertexShader.shader", "../shaders/GroundFragmentShader.shader");
	GLuint playerProgramID = LoadShaders("../shaders/PlayerVertexShader.shader", "../shaders/PlayerFragmentShader.shader");

	double t;

	GLuint scale_uniform_location = glGetUniformLocation(programID, "scale");
	GLuint scale_frag_uniform_location = glGetUniformLocation(programID, "scale_frag");
	GLuint width_uniform_location = glGetUniformLocation(programID, "width");
	GLuint height_uniform_location = glGetUniformLocation(programID, "height");
	GLuint mode_location = glGetUniformLocation(programID, "mode");
	GLuint model_location = glGetUniformLocation(programID, "model");
	GLuint view_location = glGetUniformLocation(programID, "view");
	GLuint proj_location = glGetUniformLocation(programID, "proj");

	GLuint scale_frag_uniform_ground_location = glGetUniformLocation(groundProgramID, "scale_frag");
	GLuint width_uniform_ground_location = glGetUniformLocation(groundProgramID, "width");
	GLuint height_uniform_ground_location = glGetUniformLocation(groundProgramID, "height");
	GLuint mode_ground_location = glGetUniformLocation(groundProgramID, "mode");

	GLuint player_model_location = glGetUniformLocation(playerProgramID, "model");
	GLuint player_view_location = glGetUniformLocation(playerProgramID, "view");
	GLuint player_proj_location = glGetUniformLocation(playerProgramID, "proj");

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	int mode = 0;

	int width, height, nrChannels;
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	GLuint texture_color_location = glGetUniformLocation(programID, "colorTexture");
	GLuint texture_color_ground_location = glGetUniformLocation(groundProgramID, "colorTexture");
	GLuint texture_color_player_location = glGetUniformLocation(playerProgramID, "colorTexture");
	GLuint textures;
	if (data) {
		glCreateTextures(GL_TEXTURE_2D, 1, &textures);
		glTextureStorage2D(textures, 1, GL_RGB8, width, height);
		glTextureSubImage2D(textures, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

		glBindTextureUnit(0, textures);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);


	glm::mat4 modelPlayer = glm::mat4(1.0f);
	// Start loop
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));


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

		glUseProgram(programID);

		glUniform1f(scale_uniform_location, scale_uniform);
		glUniform1f(scale_frag_uniform_location, scale_uniform);
		glUniform1i(width_uniform_location, w);
		glUniform1i(height_uniform_location, h);
		glUniform1i(mode_location, mode);
		glUniform1i(texture_color_location, 0);
		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));


		//Loop body // Draw here
		//glEnableVertexAttribArray(0);

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
			(void*)offsetof(vertex_struct, vertex_struct::color) // array buffer offset
		);
		glEnableVertexAttribArray(colorLocation);

		GLint uvLocation = glGetAttribLocation(programID, "uv");
		glVertexAttribPointer(
			uvLocation,			// attribute 0. No particular reason for 0, but must match the layout in the shader.
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

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		



		glUseProgram(groundProgramID);

		glUniform1f(scale_frag_uniform_ground_location, scale_uniform);
		glUniform1i(width_uniform_ground_location, w);
		glUniform1i(height_uniform_ground_location, h);
		glUniform1i(mode_ground_location, mode);
		glUniform1i(texture_color_ground_location, 0);

		//glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		positionLocation = glGetAttribLocation(groundProgramID, "vertexPosition_modelspace");
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

		colorLocation = glGetAttribLocation(groundProgramID, "color");
		glVertexAttribPointer(
			colorLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::color)			// array buffer offset
		);
		glEnableVertexAttribArray(colorLocation);

		uvLocation = glGetAttribLocation(groundProgramID, "uv");
		glVertexAttribPointer(
			uvLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::uv)			// array buffer offset
		);
		glEnableVertexAttribArray(uvLocation);

		normalLocation = glGetAttribLocation(groundProgramID, "normal");
		glVertexAttribPointer(
			normalLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::normal)			// array buffer offset
		);
		glEnableVertexAttribArray(normalLocation);

		textureCoordinatesLocation = glGetAttribLocation(groundProgramID, "aTexCoord");
		glVertexAttribPointer(
			textureCoordinatesLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::texCoord)			// array buffer offset
		);
		glEnableVertexAttribArray(textureCoordinatesLocation);

		glDrawArrays(GL_TRIANGLES, vertices.size(), vertices.size()+ground.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle



		glUseProgram(playerProgramID);

		
		if (glfwGetKey(window, GLFW_KEY_D)) {
			glm::translate(modelPlayer, glm::vec3(0.3f, 0.0f, 0.0f));
		}

		glUniform1i(texture_color_player_location, 0);
		glUniformMatrix4fv(player_model_location, 1, GL_FALSE, glm::value_ptr(modelPlayer));
		glUniformMatrix4fv(player_proj_location, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(player_view_location, 1, GL_FALSE, glm::value_ptr(view));


		//Loop body // Draw here
		//glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		positionLocation = glGetAttribLocation(playerProgramID, "vertexPosition_modelspace");
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

		colorLocation = glGetAttribLocation(playerProgramID, "color");
		glVertexAttribPointer(
			colorLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::color) // array buffer offset
		);
		glEnableVertexAttribArray(colorLocation);

		uvLocation = glGetAttribLocation(playerProgramID, "uv");
		glVertexAttribPointer(
			uvLocation,			// attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::uv)			// array buffer offset
		);
		glEnableVertexAttribArray(uvLocation);

		normalLocation = glGetAttribLocation(playerProgramID, "normal");
		glVertexAttribPointer(
			normalLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::normal)			// array buffer offset
		);
		glEnableVertexAttribArray(normalLocation);

		textureCoordinatesLocation = glGetAttribLocation(playerProgramID, "aTexCoord");
		glVertexAttribPointer(
			textureCoordinatesLocation,					// attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,					// size
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			sizeof(vertex_struct),					// stride
			(void*)offsetof(vertex_struct, vertex_struct::texCoord)			// array buffer offset
		);
		glEnableVertexAttribArray(textureCoordinatesLocation);

		glDrawArrays(GL_TRIANGLES, vertices.size()+ground.size(), vertices.size() + ground.size() + player.size());
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	return 0;
}