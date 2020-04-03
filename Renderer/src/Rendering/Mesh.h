#pragma once
#include "glm.hpp"
#include "glad/glad.h"

class Mesh
{
public:
	struct Data
	{
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> UVs;
		std::vector<unsigned int> Indices;
	};

public:
	Mesh();
	Mesh(const std::string& name, const Data& meshData);
	Mesh(const Mesh& rhs);
	virtual ~Mesh();
	virtual void Draw();
	virtual const std::string& GetName() const;

protected:
	virtual void SetupMesh();
	virtual void CreateVAO();
	virtual void BindVAO();
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
