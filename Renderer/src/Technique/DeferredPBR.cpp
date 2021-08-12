#include "Rendererpch.h"
#include "DeferredPBR.h"
#include "Utility/Filepath.h"

DeferredPBR::DeferredPBR(const Window & window) :
	window(window),
	lamp(Shader(Filepath::DeferredShader + "ADS/DeferredLamp.vs", Filepath::DeferredShader + "ADS/DeferredLamp.fs")),
	geometry(Shader(Filepath::DeferredShader + "PBR/GBuffer.vert.glsl", Filepath::DeferredShader + "PBR/GBuffer.frag.glsl")),
	deferredLighting(Shader(Filepath::DeferredShader + "PBR/Lighting.vert.glsl", Filepath::DeferredShader + "PBR/Lighting.frag.glsl"))
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

	FXAA::Parameters fxaaParameters = deferredParameters.FxaaParameters;
	fxaaParameters.Resolution = glm::ivec2(parameters.Width, parameters.Height);
	fxaa.Initialize(fxaaParameters);

	bloom.Initialize(parameters);
	ssao.Initialize(parameters, scene.GetCamera().GetProjectionMatrix());
}

void DeferredPBR::Render(Scene & scene)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, window.GetWindowParameters().Width, window.GetWindowParameters().Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glm::mat4 view = scene.GetCamera().GetViewMatrix();

	GeometryPass(view, scene);

	ssao.Apply(gBufferTextures[0], gBufferTextures[1], deferredParameters.SsaoParameters);

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

	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	scene.GetSkybox().Draw();
	glDepthFunc(GL_LESS);
	bloom.Apply(deferredParameters.BloomParameters);
	fxaa.Bind();
	bloom.Draw();
	fxaa.Unbind();
	fxaa.Apply(deferredParameters.FxaaParameters);
}

void DeferredPBR::SetupGBuffers(const Window::Parameters & parameters)
{
	gBuffer.Generate();
	gBuffer.Bind();

	//Positions
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
	gBufferTextures[2] = Texture::CreateEmpty("Albedo", parameters.Width, parameters.Height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBufferTextures[2].GetID());
	//Metallic, Roughness, AO, Depth
	gBufferTextures[3] = Texture::CreateEmpty("MRAO", parameters.Width, parameters.Height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
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
	geometry.SetInt("Material.Albedo", 0);
	geometry.SetInt("Material.Normal", 1);
	geometry.SetInt("Material.MRAO", 2);

	skyboxShader.Use();
	skyboxShader.SetInt("environmentMap", 0);
	skyboxShader.SetMat4("projection", projection);

	lamp.Use();
	lamp.SetMat4("projection", projection);

	deferredLighting.Use();
	deferredLighting.SetInt("GBuffer.gPosition", 0);
	deferredLighting.SetInt("GBuffer.gNormal", 1);
	deferredLighting.SetInt("GBuffer.gAlbedo", 2);
	deferredLighting.SetInt("GBuffer.gMRAO", 3);
	deferredLighting.SetInt("IBL.Irradiance", 4);
	deferredLighting.SetInt("IBL.Prefilter", 5);
	deferredLighting.SetInt("IBL.BrdfLUT", 6);
	deferredLighting.SetInt("ssaoTexture", 7);
}

void DeferredPBR::GeometryPass(const glm::mat4 & view, Scene & scene)
{
	gBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	geometry.Use();
	const Camera& camera = scene.GetCamera();
	glm::mat4 projection = camera.GetProjectionMatrix();
	geometry.SetMat4("Matrix.Projection", projection);

	const std::vector<Actor>& actors = scene.GetActors();
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		if (actors[i].GetRenderComponent().GetPBRParameters().IsTransparent)
		{
			continue;
		}
		glm::mat4 vm = view * actors[i].GetWorldMatrix();
		geometry.SetMat4("Matrix.ModelView", vm);
		geometry.SetMat4("Matrix.MVP", projection * vm);
		geometry.SetMat3("Matrix.Normal", glm::mat3(vm));

		const Material& material = actors[i].GetRenderComponent().GetMaterial();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Albedo).GetID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Normal).GetID());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::MRAO).GetID());
		glActiveTexture(GL_TEXTURE0);

		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	gBuffer.Unbind();
}

void DeferredPBR::LightingPass(const std::vector<Light>& lights, Scene & scene)
{
	glClear(GL_COLOR_BUFFER_BIT);

	deferredLighting.Use();
	deferredLighting.SetInt("NumberOfLights", scene.GetNumberOfLights());
	deferredLighting.SetMat4("InverseView", glm::inverse(scene.GetCamera().GetViewMatrix()));
	
	//Binding GBuffer
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
	skybox.GetLookup().Bind(deferredLighting, static_cast<Texture::Type>(6));
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, ssao.GetTexture().GetID());

	glm::mat4 view = scene.GetCamera().GetViewMatrix();

	for (unsigned int i = 0; i < scene.GetNumberOfLights(); ++i)
	{
		std::string lightPosition = std::string("Lights[") + std::to_string(i) + std::string("].Position");
		std::string lightIntensity = std::string("Lights[") + std::to_string(i) + std::string("].Intensity");

		const Light::Parameters& parameters = lights[i].GetParameters();
		deferredLighting.SetVec4(lightPosition, view * glm::vec4(lights[i].GetWorldPosition(),1.0f));
		deferredLighting.SetVec3(lightIntensity, parameters.Colour);
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
