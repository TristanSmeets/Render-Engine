#pragma once
#include "glad/glad.h"
#include "glm.hpp"

class Mesh
{
public:
	struct Data
	{
		std::vector<glm::vec3> Vertices;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> UVs;
		std::vector<unsigned int> Indices;
		Data();
		Data(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs, std::vector<unsigned int> indices);
	};

public:
	Mesh();
	Mesh(const std::string& name, const Data& meshData);
	Mesh(const Mesh& rhs);
	virtual ~Mesh();
	virtual void Draw() const;
	virtual const std::string& GetName() const;

protected:
	virtual void SetupMesh();
	virtual void CreateVAO();
	virtual void BindVAO() const;
	virtual void CreateBuffer(GLuint& buffer);
	template<typename T>
	void LoadBufferData(GLenum format, const std::vector<T>& vector, GLuint& buffer);

	Data data;
	std::string name = "Empty";
	GLuint VAO = 0;
	GLuint positionBuffer = 0;
	GLuint normalBuffer = 0;
	GLuint uvBuffer = 0;
	GLuint indicesBuffer = 0;
};

template<typename T>
inline void Mesh::LoadBufferData(GLenum format, const std::vector<T>& vector, GLuint & buffer)
{
	glBindBuffer(format, buffer);
	glBufferData(format, vector.size() * sizeof(T), vector.data(), GL_STATIC_DRAW);
}
