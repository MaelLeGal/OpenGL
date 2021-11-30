#include "Mesh.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

//#include "Debug.h"
//#include "Helper.h"

Mesh::Mesh(const std::string& path)
{
	if (!LoadFromFile(path))
	{
		fprintf(stderr, "Can't load obj\n");
		return;
	}
	fprintf(stdin, "Mesh loaded\n");
}

Mesh::Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices)
{
	for (int i = 0; i < vertices.size(); i += 3)
	{
		this->vertices.push_back(Vertex{
			{vertices[i + 0], vertices[i + 1], vertices[i + 2]},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f}});
	}

	this->indices = indices;

	fprintf(stdin, "Mesh copied\n");
}

struct FaceVert
{
    FaceVert() : vertex(-1), normal(-1), uv(-1) {}

    int vertex;
    int normal;
    int uv;
};

struct VertexLess {
    bool operator() (const FaceVert& lhs, const FaceVert& rhs) const
    {
        if (lhs.vertex != rhs.vertex) return (lhs.vertex < rhs.vertex);
        if (lhs.normal != rhs.normal) return (lhs.normal < rhs.normal);
        if (lhs.uv != rhs.uv) return (lhs.uv < rhs.uv);
        return false;
    }
};

bool Mesh::LoadFromFile(const std::string& path)
{
	std::ifstream in;
	in.open(path, std::ifstream::in);

	if (in.fail())
	{
		std::cerr << "Failed to open " << path << std::endl;
		return false;
	}

	vertices.clear();
	indices.clear();
	textures.clear();

	std::vector<std::vector<float>> tmpVertices;
	std::vector<std::vector<float>> tmpNormals;
	std::vector<std::vector<float>> tmpUvs;

	std::cerr << "Loading mesh..." << std::endl;

	std::map<FaceVert, int, VertexLess> uniqueVertices;
	uint32_t vertexCount = 0;
	std::string line;
	bool isBlenderOBJ = false;

	while(!in.eof())
	{
		std::getline(in, line);
		std::istringstream iss(line);

		char trash;
		if (!line.compare(0, 1, "#"))
		{
			isBlenderOBJ = true;
		}
		else if (!line.compare(0, 2, "v "))
		{
			iss >> trash;
			std::vector<float> vertex;
			float w = 1.f;
			for (int i = 0; i <= 3; i++)
			{
				iss >> (i == 3 ? w : vertex[i]);
			}
			tmpVertices.push_back(std::vector<float>{vertex[0] / w, vertex[3] / w, vertex[2] / w});
		}
		else if (!line.compare(0, 3, "vt "))
		{
			iss >> trash;
			std::vector<float> uv;			
			for (int i = 0; i < 2; i++)
			{
				iss >> uv[i];
			}
			tmpUvs.push_back(uv);
		}
		else if (!line.compare(0, 3, "vn "))
		{
			iss >> trash;
			std::vector<float> normal;			
			for (int i = 0; i < 3; i++)
			{
				iss >> normal[i];
			}
			tmpNormals.push_back(normal);
		}
		else if (!line.compare(0, 2, "f "))
		{
			std::vector<int32_t> vertexIndices;
			std::vector<int32_t> normalIndices;
			std::vector<int32_t> uvIndices;

			auto subLine= line.substr(2);
			const char* linePtr = subLine.c_str();
			while (linePtr[0] != 0)
			{
				while(linePtr[0] == ' ') ++linePtr;

				int vi = 0, ni = 0, uvi = 0;

				if (sscanf_s(linePtr, "%d/%d/%d", &vi, !isBlenderOBJ ? &ni : &uvi, !isBlenderOBJ ? &uvi : &ni) == 3)
				{
					vertexIndices.push_back(vi - 1); 
					normalIndices.push_back(ni - 1); 
					uvIndices.push_back(uvi - 1); 
				}
				else if (sscanf_s(linePtr, "%d//%d", &vi, &ni) == 2)
				{
					vertexIndices.push_back(vi - 1); 
					normalIndices.push_back(ni - 1);
				}
				else if (sscanf_s(linePtr, "%d/%d", &vi, &uvi) == 2)
				{
					vertexIndices.push_back(vi - 1); 
					uvIndices.push_back(uvi - 1); 
				}
				else if (sscanf_s(linePtr, "%d", &vi) == 1)
				{
					vertexIndices.push_back(vi - 1); 
				}

				while(linePtr[0] != ' ' && linePtr[0] != 0) ++linePtr;
			}
			
			for (uint32_t i = 1; i < vertexIndices.size() - 1; ++i)
			{
				FaceVert tri;
				tri.vertex = vertexIndices[0];
				if (!normalIndices.empty()) tri.normal = normalIndices[0];
				if (!uvIndices.empty()) tri.uv = uvIndices[0];

				if (uniqueVertices.count(tri) == 0) uniqueVertices[tri] = vertexCount++;
				indices.push_back(uniqueVertices[tri]);
								
				tri.vertex = vertexIndices[i];
				if (!normalIndices.empty()) tri.normal = normalIndices[i];
				if (!uvIndices.empty()) tri.uv = uvIndices[i];
				
				if (uniqueVertices.count(tri) == 0) uniqueVertices[tri] = vertexCount++;
				indices.push_back(uniqueVertices[tri]);

				tri.vertex = vertexIndices[i + 1];
				if (!normalIndices.empty()) tri.normal = normalIndices[i + 1];
				if (!uvIndices.empty()) tri.uv = uvIndices[i + 1];
				
				if (uniqueVertices.count(tri) == 0) uniqueVertices[tri] = vertexCount++;
				indices.push_back(uniqueVertices[tri]);
			}
		}
	}

	in.close();

	vertices.reserve(vertexCount);
	vertices.resize(vertexCount);
	std::map<FaceVert, int, VertexLess>::iterator iter;
	for (iter = uniqueVertices.begin(); iter != uniqueVertices.end(); ++iter)
	{
		auto position = tmpVertices[iter->first.vertex];
		auto normal = iter->first.normal != -1 && !tmpNormals.empty()
			? tmpNormals[iter->first.normal] : std::vector<float>{0.0f, 0.0f, 0.0f};
		auto uv = iter->first.uv != -1 && !tmpNormals.empty()
			? tmpUvs[iter->first.uv] : std::vector<float>{0.0f, 0.0f};

		vertices[iter->second] = Vertex{ position, normal, uv };
	}

	std::cerr << "# v# " << vertices.size() << " f# " << indices.size() / 3 << std::endl;

	std::vector<float> min, max;
	if (!vertices.empty())
	{
		min = vertices[0].position;
		min = max = vertices[0].position;
		for (uint32_t i = 1; i < vertices.size(); ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				min[j] = std::min(min[j], vertices[i].position[j]);			
				max[j] = std::max(max[j], vertices[i].position[j]);			
			}
		}
	}
	else
	{
		std::cerr << "Vertices Array size is less or equal to 0" << std::endl;
	}

	//std::vector<float> size = {max[0] - min[0], max[1] - min[1], max[2] - min[2] };
	//glm::vec3 center = min + glm::normalize(size) * (glm::length(size) / 2);
	//bounds = Bounds(center, size);

	return true;
}

