#pragma once
#include "glad/glad.h"

class NDCQuad
{
public:
	NDCQuad();
	virtual ~NDCQuad();
	void Render();

private:
	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
	float quadVertices[20] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
};