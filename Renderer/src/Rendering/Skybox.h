#pragma once

#include "Rendering/Mesh.h"
#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"
#include "Utility/Cubemap.h"
#include "Utility/NDCQuad.h"
#include "Rendering/Texture.h"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

class Skybox
{
public:
	Skybox();
	~Skybox();
	void LoadHDR(const std::string& filepath);
	Framebuffer& GetFramebuffer();
	Renderbuffer& GetRenderbuffer();
	Cubemap& GetEnvironment();
	void Draw();

private:
	void ConvertHDRTextureToCubemap(Texture&& texture);
	void CreateIrradianceMap();
	void CreatePrefilterMap();
	void CreateLookupTexture();

	Cubemap irradiance;
	Cubemap prefilter;
	Texture lookup;

	Mesh skyboxMesh;
	Framebuffer framebuffer;
	Renderbuffer renderbuffer;
	Cubemap environment;
	NDCQuad ndcQuad;

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[6] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
};