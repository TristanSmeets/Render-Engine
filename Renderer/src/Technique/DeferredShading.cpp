#include "Rendererpch.h"
#include "DeferredShading.h"
#include "Utility/Filepath.h"
#include <random>

DeferredShading::DeferredShading(const Window& window) :
	window(window),
	lamp(Filepath::DeferredShader + "DeferredLamp.vs", Filepath::DeferredShader + "DeferredLamp.fs"),
	lightingShader(Filepath::DeferredShader + "DeferredShading.vs", Filepath::DeferredShader + "DeferredShading.fs"),
	geometryShader(Filepath::DeferredShader + "GBuffer.vs", Filepath::DeferredShader + "GBuffer.fs")
{
}

DeferredShading::~DeferredShading()
{
}

void DeferredShading::Initialize(Scene & scene)
{
	//GBuffer setup
	gBuffer.Generate();
	gBuffer.Bind();
	
	Window::Parameters parameters = window.GetWindowParameters();
	//Position Colour Buffer
	gBufferTextures[0] = Texture::CreateEmpty("PositionColour", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBufferTextures[0].GetID());
	//Normal Colour Buffer
	gBufferTextures[1] = Texture::CreateEmpty("NormalColour", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBufferTextures[1].GetID());
	//Colour + Specular Colour buffer
	gBufferTextures[2] = Texture::CreateEmpty("AlbedoSpecular", parameters.Width, parameters.Height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gBuffer.AttachTexture(GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBufferTextures[2].GetID());
	
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	renderbuffer.Generate();
	renderbuffer.Bind();
	renderbuffer.SetStorage(GL_DEPTH_COMPONENT, parameters.Width, parameters.Height);
	gBuffer.AttachRenderbuffer(GL_DEPTH_ATTACHMENT, renderbuffer);
	if (!gBuffer.IsCompleted())
	{
		printf("ERROR: G-Buffer not complete\n");
	}
	gBuffer.Unbind();

	//SSAO buffer setup
	aoColourBuffer.Generate();
	aoBlurBuffer.Generate();
	//Colour buffer
	aoColourBuffer.Bind();
	aoColour = Texture::CreateEmpty("aoColour", parameters.Width, parameters.Width, GL_RED, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	aoColourBuffer.AttachTexture(aoColour);
	if (!aoBlurBuffer.IsCompleted())
	{
		printf("SSAO Framebuffer not complete\n");
	}
	//Blur buffer
	aoBlurBuffer.Bind();
	aoBlur = Texture::CreateEmpty("aoBlur", parameters.Width, parameters.Height, GL_RED, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	aoBlurBuffer.AttachTexture(aoBlur);
	if (!aoBlurBuffer.IsCompleted())
	{
		printf("SSAO Blur Framebuffer not complete\n");
	}
	aoBlurBuffer.Unbind();

	//Generate sample kernel
	std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		//Scale samples so there more alligned to the center of the kernel
		scale = 0.1f + (scale * scale) * (1.0f - 0.1f);
		sample *= scale;
		ssaoKernel[i] = sample;
	}

	//Generate noise Texture
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
	aoNoise = Texture("aoNoise", noiseTexture);

	//Shader setup
	glm::mat4 projection = scene.GetCamera().GetProjectionMatrix();

	lightingShader.Use();
	lightingShader.SetInt("gPosition", 0);
	lightingShader.SetInt("gNormal", 1);
	lightingShader.SetInt("gAlbedoSpecular", 2);

	geometryShader.Use();
	geometryShader.SetInt("diffuse", 0);
	geometryShader.SetInt("specular", 1);
	geometryShader.SetMat4("projection", projection);

	skyboxShader.Use();
	skyboxShader.SetInt("environmentMap", 0);
	skyboxShader.SetMat4("projection", projection);

	lamp.Use();
	lamp.SetMat4("projection", projection);

	glViewport(0, 0, parameters.Width, parameters.Height);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void DeferredShading::Render(Scene & scene)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Geometry pass
	gBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = scene.GetCamera().GetViewMatrix();

	geometryShader.Use();
	geometryShader.SetMat4("view", view);

	const std::vector<Actor>& actors = scene.GetActors();
	
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		geometryShader.SetMat4("model", actors[i].GetWorldMatrix());
		const Material& material = actors[i].GetRenderComponent().GetMaterial();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Albedo).GetID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Metallic).GetID());

		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	gBuffer.Unbind();

	//Lighting pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	lightingShader.Use();
	for (unsigned int i = 0; i < 3; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[i].GetID());
	}
	//Send light relevant uniforms
	const std::vector<Light>& lights = scene.GetLights();
	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		lightingShader.SetVec3("lights[" + std::to_string(i) + "].Position", lights[i].GetWorldPosition());
		lightingShader.SetVec3("lights[" + std::to_string(i) + "].Color", lights[i].GetColour());
		//Update attenuation parameters and calculate radius
		glm::vec3 colour = lights[i].GetColour();
		const float maxBrightness = std::fmaxf(std::fmaxf(colour.r, colour.g), colour.b);
		float radius = (-attenuationLinear + std::sqrtf(attenuationLinear * attenuationLinear - 4 * attenuationQuadratic * (attenuationConstant - (256.0f / 10.0f) * maxBrightness))) / (2.0f * attenuationQuadratic);
		lightingShader.SetFloat("lights[" + std::to_string(i) + "].Linear", attenuationLinear);
		lightingShader.SetFloat("lights[" + std::to_string(i) + "].Quadratic", attenuationQuadratic);
		lightingShader.SetFloat("lights[" + std::to_string(i) + "].Radius", radius);

	}
	lightingShader.SetVec3("viewPosition", scene.GetCamera().GetWorldPosition());

	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);

	//Copy content of geometry shader to default framebuffer's depth buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.GetBuffer());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format)
	Window::Parameters parameter = window.GetWindowParameters();

	glBlitFramebuffer(0, 0, parameter.Width, parameter.Height, 0, 0, parameter.Width, parameter.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	gBuffer.Unbind();

	//Render lights on top of scene
	lamp.Use();
	lamp.SetMat4("view", view);

	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		lamp.SetMat4("model", lights[i].GetWorldMatrix());
		lamp.SetVec3("lightColour", lights[i].GetColour());
		lights[i].GetRenderComponent().GetMesh().Draw();
	}

	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	scene.GetSkybox().Draw();
	glDepthFunc(GL_LESS);
}
