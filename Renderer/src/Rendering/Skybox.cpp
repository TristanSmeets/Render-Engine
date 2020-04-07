#include "Rendererpch.h"
#include "Skybox.h"

#include "glad/glad.h"
#include "Rendering/Shader.h"
#include "Utility/Filepath.h"
#include "stb_image.h"
#include "Utility/MeshLoader.h"

Skybox::Skybox()
{
}

Skybox::~Skybox()
{
	printf("Destroying Skybox\n");
	//TODO: Implement deletion of things that need to be deleted.
}

void Skybox::Initialize()
{
	const std::vector<Mesh>& skyboxMeshes = MeshLoader::LoadModel(Filepath::Mesh + "Skybox.obj");
	skyboxMesh = skyboxMeshes[0];
	const std::vector<Mesh>& quadMeshes = MeshLoader::LoadModel(Filepath::Mesh + "Quad.obj");
	quadMesh = quadMeshes[0];

	framebuffer.Generate();
	renderbuffer.Generate();
	framebuffer.Bind();
	renderbuffer.Bind();
	renderbuffer.SetStorage(GL_DEPTH_COMPONENT24, 512, 512);
	framebuffer.AttachRenderbuffer(GL_DEPTH_ATTACHMENT, renderbuffer);
	environment.CreateTexture(renderbuffer.GetWidth(), renderbuffer.GetHeight(), GL_RGB16F, GL_RGB, GL_FLOAT);
}

void Skybox::LoadHDR(const std::string & filepath)
{
	printf("Loading HDR texture: %s\n", filepath.c_str());
	ConvertHDRTextureToCubemap(Texture(filepath, GL_RGB16F, GL_RGB, GL_FLOAT));
	CreateIrradianceMap();
	CreatePrefilterMap();
	CreateLookupTexture();
}

Framebuffer & Skybox::GetFramebuffer()
{
	return framebuffer;
}

Renderbuffer & Skybox::GetRenderbuffer()
{
	return renderbuffer;
}

const Cubemap & Skybox::GetEnvironment() const
{
	return environment;
}

const Cubemap & Skybox::GetIrradiance() const
{
	return irradiance;
}

const Cubemap & Skybox::GetPrefilter() const
{
	return prefilter;
}

const Texture & Skybox::GetLookup() const
{
	return lookup;
}

void Skybox::Draw() const
{
	glActiveTexture(GL_TEXTURE0);
	environment.Bind();
	//irradiance.Bind();
	//prefilter.Bind();
	skyboxMesh.Draw();
}

void Skybox::ConvertHDRTextureToCubemap(Texture && texture)
{
	Shader equirectangularToCubemap(Filepath::Shader + "Cubemap.vs",
		Filepath::Shader + "EquirectangularToCubeMap.fs");

	equirectangularToCubemap.Use();
	equirectangularToCubemap.SetInt("equirectangularMap", 0);
	equirectangularToCubemap.SetMat4("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.GetID());

	glViewport(0, 0, environment.GetWidth(), environment.GetHeight());
	framebuffer.Bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemap.SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environment.GetID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skyboxMesh.Draw();
	}
	framebuffer.Unbind();
	environment.Bind();
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void Skybox::CreateIrradianceMap()
{
	irradiance.CreateTexture(32, 32, GL_RGB16F, GL_RGB, GL_FLOAT);
	framebuffer.Bind();
	renderbuffer.Bind();
	renderbuffer.SetStorage(GL_DEPTH_COMPONENT24, irradiance.GetWidth(), irradiance.GetHeight());

	Shader irradianceShader(Filepath::Shader + "Cubemap.vs", Filepath::Shader + "IrradianceConvolution.fs");
	irradianceShader.Use();
	irradianceShader.SetInt("environmentMap", 0);
	irradianceShader.SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	environment.Bind();

	glViewport(0, 0, irradiance.GetWidth(), irradiance.GetHeight());
	framebuffer.Bind();

	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance.GetID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skyboxMesh.Draw();
	}
	framebuffer.Unbind();
}

void Skybox::CreatePrefilterMap()
{
	prefilter.CreateTexture(128, 128, GL_RGB16F, GL_RGB, GL_FLOAT);
	prefilter.SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	Shader prefilterShader(Filepath::Shader + "Cubemap.vs", Filepath::Shader + "Prefilter.fs");
	prefilterShader.Use();
	prefilterShader.SetInt("environmentMap", 0);
	prefilterShader.SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	environment.Bind();
	framebuffer.Bind();

	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = (unsigned int)(prefilter.GetWidth() * std::pow(0.5f, mip));
		unsigned int mipHeight = (unsigned int)(prefilter.GetHeight() * std::pow(0.5f, mip));

		renderbuffer.Bind();
		renderbuffer.SetStorage(GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.SetFloat("roughness", roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter.GetID(), mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			skyboxMesh.Draw();
		}
	}
	framebuffer.Unbind();
}

void Skybox::CreateLookupTexture()
{
	lookup = Texture::CreateEmpty("Lookup", 512, 512, GL_RG16F, GL_RG, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	framebuffer.Bind();
	renderbuffer.Bind();
	renderbuffer.SetStorage(GL_DEPTH_COMPONENT24, 512, 512);
	framebuffer.AttachTexture(lookup);

	glViewport(0, 0, 512, 512);

	Shader brdf(Filepath::Shader + "brdf.vs", Filepath::Shader + "brdf.fs");
	brdf.Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//quadMesh.Draw();

	framebuffer.Unbind();
}
