#include "Rendererpch.h"
#include "DeferredPBR.h"
#include "Utility/Filepath.h"

DeferredPBR::DeferredPBR(const Window & window) :
	window(window),
	lamp(Shader(Filepath::DeferredShader + "ADS/DeferredLamp.vs", Filepath::DeferredShader + "ADS/DeferredLamp.fs")),
	geometry(Shader(Filepath::DeferredShader + "PBR/GBuffer.vs", Filepath::DeferredShader + "PBR/GBuffer.fs")),
	pbrLighting(Shader(Filepath::DeferredShader + "PBR/PBRLighting.vs", Filepath::DeferredShader + "PBR/PBRLighting.fs")),
	forwardLighting(Shader(Filepath::ForwardShader + "PBR.vs", Filepath::ForwardShader + "PBR.fs"))
{
}

DeferredPBR::~DeferredPBR()
{
}

void DeferredPBR::Initialize(Scene & scene)
{
	const Window::Parameters parameters = window.GetWindowParameters();
	SetupGBuffers(parameters);

	ShadowMapping::Parameters shadowParameters;
	shadowParameters.AspectRatio = 1.0f;
	shadowParameters.Resolution = glm::vec2(1024, 1024);
	shadowMapping.Initialize(shadowParameters);

	SetupShaders(scene);

	glViewport(0, 0, parameters.Width, parameters.Height);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	FXAA::Parameters fxaaParameters = deferredParameters.FxaaParameters;
	fxaaParameters.Resolution = glm::ivec2(parameters.Width, parameters.Height);
	fxaa.Initialize(fxaaParameters);

	bloom.Initialize(parameters);
	depthOfField.Initialize(parameters);
	ssao.Initialize(parameters, scene.GetCamera().GetProjectionMatrix());
}

void DeferredPBR::Render(Scene & scene)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shadowMapping.MapPointLights(scene.GetLights(), scene.GetActors(), scene.GetNumberOfLights());

	glViewport(0, 0, window.GetWindowParameters().Width, window.GetWindowParameters().Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glm::mat4 view = scene.GetCamera().GetViewMatrix();

	GeometryPass(view, scene);

	ssao.Apply(gBufferTextures[0], gBufferTextures[4], deferredParameters.SsaoParameters);

	Framebuffer::BlitParameters blitParameters;
	blitParameters.Destination = &bloom.GetFramebuffer();
	blitParameters.Resolution = glm::ivec2(window.GetWindowParameters().Width, window.GetWindowParameters().Height);
	blitParameters.Mask = GL_DEPTH_BUFFER_BIT;
	blitParameters.Filter = GL_NEAREST;
	gBuffer.BlitFramebuffer(blitParameters);

	//Lighting pass
	const std::vector<Light>& lights = scene.GetLights();
	LightingPass(lights, scene);
	RenderLights(view, lights, scene.GetNumberOfLights());
	RenderTransparentActors(scene);

	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	scene.GetSkybox().Draw();
	glDepthFunc(GL_LESS);
	bloom.Apply(deferredParameters.BloomParameters);
	depthOfField.Bind();
	bloom.Draw();
	depthOfField.Apply(deferredParameters.DofParamaters);
	fxaa.Bind();
	depthOfField.Draw(gBufferTextures[3]);
	fxaa.Unbind();
	fxaa.Apply(deferredParameters.FxaaParameters);
}

void DeferredPBR::SetupGBuffers(const Window::Parameters & parameters)
{
	gBuffer.Generate();
	gBuffer.Bind();

	//View positions
	gBufferTextures[0] = Texture::CreateEmpty("ViewPositions", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBufferTextures[0].GetID());
	//Normal Colour buffer
	gBufferTextures[1] = Texture::CreateEmpty("Normals", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBufferTextures[1].GetID());
	//Albedo
	gBufferTextures[2] = Texture::CreateEmpty("Albedo", parameters.Width, parameters.Height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBufferTextures[2].GetID());
	//Metallic, Roughness, AO, Depth
	gBufferTextures[3] = Texture::CreateEmpty("MetallicRoughnessAO", parameters.Width, parameters.Height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gBufferTextures[3].GetID());
	//View Normals
	gBufferTextures[4] = Texture::CreateEmpty("ViewNormals", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gBufferTextures[4].GetID());

	GLuint attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

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
	for (int i = 0; i < shadowMapping.GetMaximumNumberOfLights(); ++i)
	{
		pbrLighting.SetInt("shadowCubeMaps[" + std::to_string(i) + "]", i + 8);
	}

	forwardLighting.Use();
	forwardLighting.SetMat4("projection", projection);
	forwardLighting.SetInt("irradianceMap", 5);
	forwardLighting.SetInt("prefilterMap", 6);
	forwardLighting.SetInt("brdfLUT", 7);
	forwardLighting.SetInt("shadowMap", 8);

	for (int i = 0; i < shadowMapping.GetMaximumNumberOfLights(); ++i)
	{
		forwardLighting.SetInt("shadowCubeMaps[" + std::to_string(i) + "]", i + 9);
	}
}

void DeferredPBR::GeometryPass(const glm::mat4 & view, Scene & scene)
{
	gBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	geometry.Use();
	geometry.SetMat4("view", view);
	geometry.SetFloat("nearPlane", scene.GetCamera().GetFrustum().NearPlaneCutoff);
	geometry.SetFloat("farPlane", scene.GetCamera().GetFrustum().FarPlaneCutoff);

	const std::vector<Actor>& actors = scene.GetActors();
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		if (actors[i].GetRenderComponent().GetPBRParameters().IsTransparent)
		{
			continue;
		}
		geometry.SetMat4("model", actors[i].GetWorldMatrix());
		const Material& material = actors[i].GetRenderComponent().GetMaterial();
		for (int j = 0; j < Texture::Count; ++j)
		{
			glActiveTexture(GL_TEXTURE0 + j);
			glBindTexture(GL_TEXTURE_2D, material.GetTexture((Texture::Type)j).GetID());
		}
		const RenderComponent::PBRParameters& pbrParameters = actors[i].GetRenderComponent().GetPBRParameters();
		geometry.SetFloat("roughness", pbrParameters.Roughness);
		Shader::SubroutineParameters subroutineParameters;
		subroutineParameters.Shader = GL_FRAGMENT_SHADER;

		if (pbrParameters.UsingSmoothness)
		{
			subroutineParameters.Name = "UsingSmoothness";
		}
		else
		{
			subroutineParameters.Name = "UsingRoughness";
		}

		geometry.SetSubroutine(subroutineParameters);
		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	gBuffer.Unbind();
}

void DeferredPBR::LightingPass(const std::vector<Light>& lights, Scene & scene)
{
	glClear(GL_COLOR_BUFFER_BIT);

	pbrLighting.Use();
	pbrLighting.SetInt("NumberOfLights", scene.GetNumberOfLights());
	pbrLighting.SetVec3("cameraPosition", scene.GetCamera().GetWorldPosition());
	pbrLighting.SetVec3("nonMetallicReflectionColour", deferredParameters.PbrParameters.NonMetallicReflectionColour);
	pbrLighting.SetFloat("farPlane", shadowMapping.GetParameters().FarPlane);
	pbrLighting.SetMat4("inverseView", glm::inverse(scene.GetCamera().GetViewMatrix()));
	pbrLighting.SetMat4("view", scene.GetCamera().GetViewMatrix());

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
	glBindTexture(GL_TEXTURE_2D, ssao.GetTexture().GetID());

	for (unsigned int i = 0; i < scene.GetNumberOfLights(); ++i)
	{
		std::string lightPosition = std::string("lights[") + std::to_string(i) + std::string("].Position");
		std::string lightColour = std::string("lights[") + std::to_string(i) + std::string("].Colour");
		std::string lightRadius = std::string("lights[") + std::to_string(i) + std::string("].Radius");

		const Light::Parameters& parameters = lights[i].GetParameters();
		pbrLighting.SetVec3(lightPosition, lights[i].GetWorldPosition());
		pbrLighting.SetVec3(lightColour, parameters.Colour);
		pbrLighting.SetFloat(lightRadius, parameters.Radius);

		//Attach shadow map
		glActiveTexture(GL_TEXTURE8 + i);
		shadowMapping.BindShadowMap(i);
	}
	glActiveTexture(GL_TEXTURE0);

	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

void DeferredPBR::RenderLights(const glm::mat4 & view, const std::vector<Light>& lights, int numberOfLights)
{
	lamp.Use();
	lamp.SetMat4("view", view);

	for (int i = 0; i < numberOfLights; ++i)
	{
		lamp.SetMat4("model", lights[i].GetWorldMatrix());
		lamp.SetVec3("lightColour", lights[i].GetColour());
		lights[i].GetRenderComponent().GetMesh().Draw();
	}
}

void DeferredPBR::RenderTransparentActors(Scene & scene)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::vector<Actor>& actors = scene.GetActors();

	std::map<float, const Actor*> distanceSortedActors;

	for (int i = 0; i < actors.size(); ++i)
	{
		if (!actors[i].GetRenderComponent().GetPBRParameters().IsTransparent)
		{
			continue;
		}
		float distance = glm::length(scene.GetCamera().GetWorldPosition() - actors[i].GetWorldPosition());
		distanceSortedActors[distance] = &actors[i];
	}

	SetPBRShaderUniforms(scene);

	for (std::map<float, const Actor*>::reverse_iterator it = distanceSortedActors.rbegin(); it != distanceSortedActors.rend(); ++it)
	{
		const Actor* actor = it->second;
		forwardLighting.SetMat4("model", actor->GetWorldMatrix());
		const RenderComponent::PBRParameters& pbrParameters = actor->GetRenderComponent().GetPBRParameters();
		forwardLighting.SetVec3("NonMetallicReflectionColour", pbrParameters.NonMetallicReflectionColour);
		forwardLighting.SetFloat("inputRoughness", pbrParameters.Roughness);
		Shader::SubroutineParameters subroutineParameters;
		subroutineParameters.Shader = GL_FRAGMENT_SHADER;

		if (pbrParameters.UsingSmoothness)
		{
			subroutineParameters.Name = "UsingSmoothness";
		}
		else
		{
			subroutineParameters.Name = "UsingRoughness";
		}

		forwardLighting.SetSubroutine(subroutineParameters);

		const Material& material = actor->GetRenderComponent().GetMaterial();
		material.GetTexture(Texture::Albedo).Bind(forwardLighting, Texture::Albedo);
		material.GetTexture(Texture::Normal).Bind(forwardLighting, Texture::Normal);
		material.GetTexture(Texture::Metallic).Bind(forwardLighting, Texture::Metallic);
		material.GetTexture(Texture::Roughness).Bind(forwardLighting, Texture::Roughness);
		material.GetTexture(Texture::AmbientOcclusion).Bind(forwardLighting, Texture::AmbientOcclusion);
		actor->GetRenderComponent().GetMesh().Draw();
	}
	glDisable(GL_BLEND);
}

void DeferredPBR::SetPBRShaderUniforms(const Scene& scene)
{
	forwardLighting.Use();
	forwardLighting.SetMat4("view", scene.GetCamera().GetViewMatrix());
	forwardLighting.SetVec3("cameraPos", scene.GetCamera().GetWorldPosition());
	forwardLighting.SetFloat("farPlane", shadowMapping.GetParameters().FarPlane);
	forwardLighting.SetInt("NumberOfLights", scene.GetNumberOfLights());
	
	const Skybox& skybox = scene.GetSkybox();
	glActiveTexture(GL_TEXTURE5);
	skybox.GetIrradiance().Bind();
	glActiveTexture(GL_TEXTURE6);
	skybox.GetPrefilter().Bind();
	skybox.GetLookup().Bind(forwardLighting, (Texture::Type)7);
	for (int i = 0; i < shadowMapping.GetMaximumNumberOfLights(); ++i)
	{
		glActiveTexture(GL_TEXTURE9 + i);
		shadowMapping.BindShadowMap(i);
	}
	glActiveTexture(GL_TEXTURE0);

	const std::vector<Light>& lights = scene.GetLights();

	for (unsigned int i = 0; i < scene.GetNumberOfLights(); ++i)
	{
		std::string lightPosition = std::string("lights[") + std::to_string(i) + std::string("].Position");
		std::string lightColour = std::string("lights[") + std::to_string(i) + std::string("].Colour");
		std::string lightRadius = std::string("lights[") + std::to_string(i) + std::string("].Radius");
		forwardLighting.SetVec3(lightPosition, lights[i].GetWorldPosition());
		forwardLighting.SetVec3(lightColour, lights[i].GetColour());
		forwardLighting.SetFloat(lightRadius, lights[i].GetParameters().Radius);
	}
}
