#include "Rendererpch.h"
#include "NDCQuad.h"

NDCQuad::NDCQuad()
{
	Data quadData;
	quadData.Positions = 
	{
		glm::vec3(-1.0f,  1.0f, 0.0f),
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(1.0f,  1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f)
	};
	quadData.UVs =
	{
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f)
	};
	quadData.Indices =
	{
		0,1,3,
		1,2,3,
	};
	Mesh("NDCQuad", quadData);
}

NDCQuad::~NDCQuad()
{
	Mesh::~Mesh();
}
