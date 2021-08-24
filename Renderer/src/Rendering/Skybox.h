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
	void Initialize();
	void LoadHDR(const std::string& filepath);
	Framebuffer& GetFramebuffer();
	Renderbuffer& GetRenderbuffer();
	const Cubemap& GetEnvironment() const;
	const Cubemap& GetIrradiance() const;
	const Cubemap& GetPrefilter() const;
	const Texture& GetLookup() const;
	void BindTexturesToShader(GLSLProgram& shader, GLuint position) const;
	void Draw() const;

private:
	void ConvertHDRTextureToCubemap(Texture&& texture);
	void CreateIrradianceMap();
	void CreatePrefilterMap();
	void CreateLookupTexture();

	NDCQuad ndcQuad;
	Cubemap irradiance;
	Cubemap prefilter;
	Texture lookup;

	Mesh skyboxMesh;
	Framebuffer framebuffer;
	Renderbuffer renderbuffer;
	Cubemap environment;

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