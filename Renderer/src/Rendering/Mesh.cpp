#include "Rendererpch.h"
#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(const std::string & name, const Data & meshData) :
	name(name), data(meshData)
{
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &indicesBuffer);
}

void Mesh::Draw()
{
	BindVAO();
	glDrawElements(GL_TRIANGLES, (GLsizei)data.Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

const std::string & Mesh::GetName() const
{
	return name;
}

void Mesh::SetupMesh()
{
	CreateVAO();
	CreateBuffer(positionBuffer);
	CreateBuffer(normalBuffer);
	CreateBuffer(uvBuffer);
	CreateBuffer(indicesBuffer);

	BindVAO();
	LoadBufferData(GL_ARRAY_BUFFER, data.Positions, positionBuffer);
	LoadBufferData(GL_ARRAY_BUFFER, data.Normals, normalBuffer);
	LoadBufferData(GL_ARRAY_BUFFER, data.UVs, uvBuffer);
	LoadBufferData(GL_ELEMENT_ARRAY_BUFFER, data.Indices, indicesBuffer);

	//Setup VAO
	//Positions
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//Normals
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//UVs
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}

void Mesh::CreateVAO()
{
	glGenVertexArrays(1, &VAO);
}

void Mesh::BindVAO()
{
	glBindVertexArray(VAO);
}

void Mesh::CreateBuffer(GLuint& buffer)
{
	glGenBuffers(1, &buffer);
}
