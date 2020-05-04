#include "Rendererpch.h"
#include "DeferredPBR.h"
#include "Utility/Filepath.h"

DeferredPBR::DeferredPBR(const Window & window) : 
	window(window),
	lamp(Shader(Filepath::DeferredShader + "ADS/DeferredLamp.vs", Filepath::DeferredShader + "ADS/DeferredLamp.fs")),
	geometry(Shader(Filepath::DeferredShader + "PBR/GBuffer.vs", Filepath::DeferredShader + "PBR/GBuffer.fs")),
	pbrLighting(Shader(Filepath::DeferredShader + "PBR/PBRLighting.vs", Filepath::DeferredShader + "PBR/PBRLighting.fs"))
{
}

DeferredPBR::~DeferredPBR()
{
}

void DeferredPBR::Initialize(Scene & scene)
{
	const Window::Parameters parameters = window.GetWindowParameters();
	SetupGBuffers(parameters);
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

	const std::vector<Light>& lights = scene.GetLights();
	LightingPass(lights, scene);
	GBufferToDefaultFramebuffer();
	RenderLights(view, lights);

	//glDepthFunc(GL_LEQUAL);
	//skyboxShader.Use();
	//skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	//scene.GetSkybox().Draw();
	//glDepthFunc(GL_LESS);
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
	gBufferTextures[2] = Texture::CreateEmpty("Albedo", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBufferTextures[2].GetID());
	//Metallic, Roughness, AO
	gBufferTextures[3] = Texture::CreateEmpty("MetallicRoughnessAO", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
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

void DeferredPBR::SetupShaders(Scene & scene)
{
	glm::mat4 projection = scene.GetCamera().GetProjectionMatrix();

	geometry.Use();
	geometry.SetInt("material.Albedo", 0);
	geometry.SetInt("material.Normal", 0);
	geometry.SetInt("material.Metallic", 0);
	geometry.SetInt("material.Roughness", 0);
	geometry.SetInt("material.AO", 0);
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
	pbrLighting.SetInt("gMetalicRoughnessAO", 3);
	pbrLighting.SetInt("irradianceMap", 4);
	pbrLighting.SetInt("prefilterMap", 5);
	pbrLighting.SetInt("brdfLUT", 6);
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
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, scene.GetActors()[1].GetRenderComponent().GetMaterial().GetTexture(Texture::Albedo).GetID());
	//glBindTexture(GL_TEXTURE_2D, gBufferTextures[0].GetID());
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
