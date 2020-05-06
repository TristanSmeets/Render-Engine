#include "Rendererpch.h"
#include "DeferredPBR.h"
#include "Utility/Filepath.h"

DeferredPBR::DeferredPBR(const Window & window) : 
	window(window),
	lamp(Shader(Filepath::DeferredShader + "ADS/DeferredLamp.vs", Filepath::DeferredShader + "ADS/DeferredLamp.fs")),
	geometry(Shader(Filepath::DeferredShader + "PBR/GBuffer.vs", Filepath::DeferredShader + "PBR/GBuffer.fs")),
	pbrLighting(Shader(Filepath::DeferredShader + "PBR/PBRLighting.vs", Filepath::DeferredShader + "PBR/PBRLighting.fs")),
	ssao(Shader(Filepath::DeferredShader + "PBR/PBRLighting.vs", Filepath::DeferredShader + "ADS/SSAO.fs")),
	ssaoBlur(Shader(Filepath::DeferredShader + "PBR/PBRLighting.vs", Filepath::DeferredShader + "ADS/SSAOBlur.fs"))
{
}

DeferredPBR::~DeferredPBR()
{
}

void DeferredPBR::Initialize(Scene & scene)
{
	const Window::Parameters parameters = window.GetWindowParameters();
	SetupGBuffers(parameters);
	SetupSSAOBuffers(parameters);

	std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;
	CreateSSAOKernel(randomFloats, generator);
	CreateNoiseTexture(randomFloats, generator);

	SetupShaders(scene);

	glViewport(0, 0, parameters.Width, parameters.Height);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void DeferredPBR::Render(Scene & scene)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = scene.GetCamera().GetViewMatrix();

	GeometryPass(view, scene);

	SSAOTexturePass();
	BlurPass();

	const std::vector<Light>& lights = scene.GetLights();
	LightingPass(lights, scene);
	GBufferToDefaultFramebuffer();
	RenderLights(view, lights);

	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	scene.GetSkybox().Draw();
	glDepthFunc(GL_LESS);
}

void DeferredPBR::SetupGBuffers(const Window::Parameters & parameters)
{
	gBuffer.Generate();
	gBuffer.Bind();

	//Position Colour buffer
	gBufferTextures[0] = Texture::CreateEmpty("Positions", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBufferTextures[0].GetID());
	//Normal Colour buffer
	gBufferTextures[1] = Texture::CreateEmpty("Normals", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBufferTextures[1].GetID());
	//Albedo
	gBufferTextures[2] = Texture::CreateEmpty("Albedo", parameters.Width, parameters.Height, GL_SRGB, GL_RGB, GL_UNSIGNED_BYTE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBufferTextures[2].GetID());
	//Metallic, Roughness, AO
	gBufferTextures[3] = Texture::CreateEmpty("MetallicRoughnessAO", parameters.Width, parameters.Height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gBufferTextures[3].GetID());

	GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	renderbuffer.Generate();
	renderbuffer.Bind();
	renderbuffer.SetStorage(GL_DEPTH_COMPONENT, parameters.Width, parameters.Height);
	gBuffer.AttachRenderbuffer(GL_DEPTH_ATTACHMENT, renderbuffer);
	if (!gBuffer.IsCompleted())
	{
		printf("ERROR: G-Buffer not complete\n");
	}
	gBuffer.Unbind();
}

void DeferredPBR::SetupSSAOBuffers(const Window::Parameters & parameters)
{
	for (int i = 0; i < 2; ++i)
	{
		aoBuffers[i].Generate();
		aoBuffers[i].Bind();
		aoTextures[i] = Texture::CreateEmpty("aoTextures#" + std::to_string(i), parameters.Width, parameters.Height, GL_RED, GL_RGB, GL_FLOAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		aoBuffers[i].AttachTexture(aoTextures[i]);
		if (!aoBuffers[i].IsCompleted())
		{
			printf("AO Buffer#%d is not complete\n", i);
		}
		aoBuffers[i].Unbind();
	}
}

void DeferredPBR::CreateSSAOKernel(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine & generator)
{
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		//scale samples so they're more aligned to the center of the kernel
		scale = 0.1f + (scale * scale) * 0.9f;
		sample *= scale;
		ssaoKernel[i] = sample;
	}
}

void DeferredPBR::CreateNoiseTexture(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine & generator)
{
	glm::vec3 ssaoNoise[16];
	for (unsigned int i = 0; i < 16; ++i)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f);
		ssaoNoise[i] = noise;
	}
	GLuint noiseTexture;
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	noise = Texture("aoNoise", noiseTexture);
}

void DeferredPBR::SetupShaders(Scene & scene)
{
	glm::mat4 projection = scene.GetCamera().GetProjectionMatrix();

	geometry.Use();
	geometry.SetInt("material.Albedo", 0);
	geometry.SetInt("material.Normal", 1);
	geometry.SetInt("material.Metallic", 2);
	geometry.SetInt("material.Roughness", 3);
	geometry.SetInt("material.AO", 4);
	geometry.SetMat4("projection", projection);

	skyboxShader.Use();
	skyboxShader.SetInt("environmentMap", 0);
	skyboxShader.SetMat4("projection", projection);

	lamp.Use();
	lamp.SetMat4("projection", projection);

	pbrLighting.Use();
	pbrLighting.SetInt("gPosition", 0);
	pbrLighting.SetInt("gNormal", 1);
	pbrLighting.SetInt("gAlbedo", 2);
	pbrLighting.SetInt("gMetallicRoughnessAO", 3);
	pbrLighting.SetInt("irradianceMap", 4);
	pbrLighting.SetInt("prefilterMap", 5);
	pbrLighting.SetInt("brdfLUT", 6);
	pbrLighting.SetInt("ssao", 7);

	ssao.Use();
	ssao.SetInt("gPosition", 0);
	ssao.SetInt("gNormal", 1);
	ssao.SetInt("noise", 2);
	ssao.SetMat4("projection", projection);

	ssaoBlur.Use();
	ssaoBlur.SetInt("ssaoInput", 0);
}

void DeferredPBR::GeometryPass(const glm::mat4 & view, Scene & scene)
{
	gBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	geometry.Use();
	geometry.SetMat4("view", view);

	const std::vector<Actor>& actors = scene.GetActors();
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		geometry.SetMat4("model", actors[i].GetWorldMatrix());
		const Material& material = actors[i].GetRenderComponent().GetMaterial();
		for (int j = 0; j < Texture::Count; ++j)
		{
			glActiveTexture(GL_TEXTURE0 + j);
			glBindTexture(GL_TEXTURE_2D, material.GetTexture((Texture::Type)j).GetID());
		}
		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	gBuffer.Unbind();
}

void DeferredPBR::SSAOTexturePass()
{
	aoBuffers[0].Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	ssao.Use();
	ssao.SetInt("occlusionPower", deferredParameters.OcclusionPower);
	ssao.SetInt("kernelSize", deferredParameters.KernelSize);
	ssao.SetFloat("radius", deferredParameters.Radius);
	ssao.SetFloat("bias", deferredParameters.Bias);

	for (unsigned int i = 0; i < 64; ++i)
	{
		ssao.SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
	}
	for (unsigned int i = 0; i < 2; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[i].GetID());
	}
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noise.GetID());
	quad.Render();
	aoBuffers[0].Unbind();
}

void DeferredPBR::BlurPass()
{
	aoBuffers[1].Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	ssaoBlur.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aoTextures[0].GetID());
	quad.Render();
	aoBuffers[1].Unbind();
}

void DeferredPBR::LightingPass(const std::vector<Light>& lights, Scene & scene)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pbrLighting.Use();
	pbrLighting.SetVec3("cameraPosition", scene.GetCamera().GetWorldPosition());
	pbrLighting.SetVec3("nonMetallicReflectionColour", pbrParameters.NonMetallicReflectionColour);
	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		std::string lightPosition = std::string("lights[") + std::to_string(i) + std::string("].Position");
		std::string lightColour = std::string("lights[") + std::to_string(i) + std::string("].Colour");
		std::string lightConstant = std::string("lights[") + std::to_string(i) + std::string("].Constant");
		std::string lightLinear = std::string("lights[") + std::to_string(i) + std::string("].Linear");
		std::string lightQuadratic = std::string("lights[") + std::to_string(i) + std::string("].Quadratic");

		const Light::Parameters& parameters = lights[i].GetParameters();
		pbrLighting.SetVec3(lightPosition, lights[i].GetWorldPosition());
		pbrLighting.SetVec3(lightColour, parameters.Colour);
		pbrLighting.SetFloat(lightConstant, parameters.Constant);
		pbrLighting.SetFloat(lightLinear, parameters.Linear);
		pbrLighting.SetFloat(lightQuadratic, parameters.Quadratic);
	}
	
	for (unsigned int i = 0; i < 4; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[i].GetID());
	}

	const Skybox& skybox = scene.GetSkybox();
	glActiveTexture(GL_TEXTURE4);
	skybox.GetIrradiance().Bind();
	glActiveTexture(GL_TEXTURE5);
	skybox.GetPrefilter().Bind();
	skybox.GetLookup().Bind(pbrLighting, (Texture::Type)6);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, aoTextures[1].GetID());

	glActiveTexture(GL_TEXTURE0);

	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

void DeferredPBR::GBufferToDefaultFramebuffer()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.GetBuffer());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	Window::Parameters parameter = window.GetWindowParameters();

	glBlitFramebuffer(0, 0, parameter.Width, parameter.Height, 0, 0, parameter.Width, parameter.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	gBuffer.Unbind();
}

void DeferredPBR::RenderLights(const glm::mat4 & view, const std::vector<Light>& lights)
{
	lamp.Use();
	lamp.SetMat4("view", view);

	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		lamp.SetMat4("model", lights[i].GetWorldMatrix());
		lamp.SetVec3("lightColour", lights[i].GetColour());
		lights[i].GetRenderComponent().GetMesh().Draw();
	}
}
