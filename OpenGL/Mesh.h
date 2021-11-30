#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#include "Bounds.h"

struct Vertex
{
	std::vector<float> position;
	std::vector<float> normal;
	std::vector<float> uv;

	bool operator ==(const Vertex& b) const
	{
		return this->position == b.position && this->normal == b.normal && this->uv == b.uv;
	}

	bool operator !=(const Vertex& b) const
	{
		return !(*this == b);
	}
};

struct Texture
{
	GLuint id;
	std::string type;

	bool operator ==(const Texture& b) const
	{
		return this->id == b.id && this->type == b.type;
	}

	bool operator !=(const Texture& b) const
	{
		return !(*this == b);
	}
};

/// <summary>
/// https://www.math.ucsd.edu/~sbuss/MathCG2/OpenGLsoft/BasicDrawModes/explainBDM.html
/// </summary>
struct Mesh
{

private:
	bool LoadFromFile(const std::string& path);

public:

	Mesh(const std::string& path);
	Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices);
	~Mesh() = default;

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	//Bounds bounds;

public:

	bool operator ==(const Mesh& b)
	{
		if (this->vertices.size() != b.vertices.size()) return false;
		if (this->indices.size() != b.indices.size()) return false;
		if (this->textures.size() != b.textures.size()) return false;

		for (size_t i = 0; i < this->vertices.size(); ++i)
		{
			if (this->vertices[i] != b.vertices[i]) return false;
		}
		for (size_t i = 0; i < this->indices.size(); ++i)
		{
			if (this->indices[i] != b.indices[i]) return false;
		}
		for (size_t i = 0; i < this->textures.size(); ++i)
		{
			if (this->textures[i] != b.textures[i]) return false;
		}

		return true;
	}

	bool operator !=(const Mesh& b)
	{
		return !(*this == b);
	}

};
