#include "Rendererpch.h"
#include "DeferredPBR.h"
#include "Utility/Filepath.h"

DeferredPBR::DeferredPBR(const Window & window) :
	window(window)
{
}

DeferredPBR::~DeferredPBR()
= default;

void DeferredPBR::Initialize(Scene & scene)
{
	lamp.CompileShader(Filepath::DeferredShader + "ADS/DeferredLamp.vs");
	lamp.CompileShader(Filepath::DeferredShader + "ADS/DeferredLamp.fs");
	lamp.Link();
	lamp.Validate();
	
	geometry.CompileShader(Filepath::DeferredShader + "PBR/GBuffer.vert.glsl");
	geometry.CompileShader(Filepath::DeferredShader + "PBR/GBuffer.frag.glsl");
	geometry.Link();
	geometry.Validate();

	deferredLighting.CompileShader(Filepath::DeferredShader + "PBR/Lighting.vert.glsl");
	deferredLighting.CompileShader(Filepath::DeferredShader + "PBR/Lighting.frag.glsl");
	deferredLighting.Link();
	deferredLighting.Validate();
	
	const Window::Parameters parameters = window.GetWindowParameters();
	SetupGBuffers(parameters);
	SetupShaders(scene);

	glViewport(0, 0, parameters.Width, parameters.Height);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	postProcessing.Initialize(parameters);
	ssao.Initialize(parameters, scene.GetCamera().GetProjectionMatrix());
}

void DeferredPBR::Render(Scene & scene)
{
	glViewport(0, 0, window.GetWindowParameters().Width, window.GetWindowParameters().Height);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = scene.GetCamera().GetViewMatrix();

	GeometryPass(view, scene);

	ssao.Apply(gBufferTextures[0], gBufferTextures[1], deferredParameters.SsaoParameters);

	// Copy depth buffer over to default frame buffer.
	Framebuffer::BlitParameters blitParameters;
	blitParameters.Destination = &postProcessing.GetFramebuffer();
	blitParameters.Resolution = glm::ivec2(window.GetWindowParameters().Width, window.GetWindowParameters().Height);
	blitParameters.Mask = GL_DEPTH_BUFFER_BIT;
	blitParameters.Filter = GL_NEAREST;
	gBuffer.BlitFramebuffer(blitParameters);

	postProcessing.Bind();

	//Lighting pass
	const std::vector<Light>& lights = scene.GetLights();
	LightingPass(lights, scene);
	RenderLights(view, lights, scene.GetNumberOfLights());

	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetUniform("view", scene.GetCamera().GetViewMatrix());
	scene.GetSkybox().Draw();
	glDepthFunc(GL_LESS);

	postProcessing.Unbind();
	postProcessing.SetUniforms(deferredParameters.PostProcessing);
	postProcessing.Apply();
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
	geometry.SetUniform("Material.Albedo", 0);
	geometry.SetUniform("Material.Normal", 1);
	geometry.SetUniform("Material.MRAO", 2);

	skyboxShader.Use();
	skyboxShader.SetUniform("environmentMap", 0);
	skyboxShader.SetUniform("projection", projection);

	lamp.Use();
	lamp.SetUniform("projection", projection);

	deferredLighting.Use();
	deferredLighting.SetUniform("GBuffer.gPosition", 0);
	deferredLighting.SetUniform("GBuffer.gNormal", 1);
	deferredLighting.SetUniform("GBuffer.gAlbedo", 2);
	deferredLighting.SetUniform("GBuffer.gMRAO", 3);
	deferredLighting.SetUniform("IBL.Irradiance", 4);
	deferredLighting.SetUniform("IBL.Prefilter", 5);
	deferredLighting.SetUniform("IBL.BrdfLUT", 6);
	deferredLighting.SetUniform("ssaoTexture", 7);
}

void DeferredPBR::GeometryPass(const glm::mat4 & view, Scene & scene)
{
	gBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	geometry.Use();
	const Camera& camera = scene.GetCamera();
	glm::mat4 projection = camera.GetProjectionMatrix();
	geometry.SetUniform("Matrix.Projection", projection);

	const std::vector<Actor>& actors = scene.GetActors();
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		glm::mat4 vm = view * actors[i].GetWorldMatrix();
		geometry.SetUniform("Matrix.ModelView", vm);
		geometry.SetUniform("Matrix.MVP", projection * vm);
		geometry.SetUniform("Matrix.Normal", glm::mat3(vm));

		actors[i].GetRenderComponent().Draw();
	}
	gBuffer.Unbind();
}

void DeferredPBR::LightingPass(const std::vector<Light>& lights, Scene & scene)
{
	glClear(GL_COLOR_BUFFER_BIT);

	deferredLighting.Use();
	deferredLighting.SetUniform("NumberOfLights", scene.GetNumberOfLights());
	deferredLighting.SetUniform("InverseView", glm::inverse(scene.GetCamera().GetViewMatrix()));
	
	//Binding GBuffer
	for (unsigned int i = 0; i < 4; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[i].GetID());
	}

	const Skybox& skybox = scene.GetSkybox();
	skybox.BindTexturesToShader(deferredLighting, 4);

	//Bind SSAO texture to shader
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, ssao.GetTexture().GetID());

	glm::mat4 view = scene.GetCamera().GetViewMatrix();

	for (unsigned int i = 0; i < scene.GetNumberOfLights(); ++i)
	{
		std::string lightPosition = std::string("Lights[") + std::to_string(i) + std::string("].Position");
		std::string lightIntensity = std::string("Lights[") + std::to_string(i) + std::string("].Intensity");

		const Light::Parameters& parameters = lights[i].GetParameters();
		deferredLighting.SetUniform(lightPosition, view * glm::vec4(lights[i].GetWorldPosition(),1.0f));
		deferredLighting.SetUniform(lightIntensity, parameters.Colour);
	}
	glActiveTexture(GL_TEXTURE0);

	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

void DeferredPBR::RenderLights(const glm::mat4 & view, const std::vector<Light>& lights, int numberOfLights)
{
	lamp.Use();
	lamp.SetUniform("view", view);

	for (int i = 0; i < numberOfLights; ++i)
	{
		lamp.SetUniform("model", lights[i].GetWorldMatrix());
		lamp.SetUniform("lightColour", lights[i].GetColour());
		lights[i].GetRenderComponent().GetMesh().Draw();
	}
}
