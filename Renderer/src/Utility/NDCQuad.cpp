#include "Rendererpch.h"
#include "NDCQuad.h"

NDCQuad::NDCQuad() : 
	Mesh()
{
}

NDCQuad::~NDCQuad()
{
	Mesh::~Mesh();
}

void NDCQuad::Initialize()
{
	data.Vertices =
	{
		glm::vec3(-1.0f,  1.0f, 0.0f),
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(1.0f,  1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f)
	};
	data.UVs =
	{
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f)
	};
	data.Indices =
	{
		0,1,3,
		1,2,3,
	};
	SetupMesh();
}
