#include "Rendererpch.h"
#include "DeferredShading.h"
#include "Utility/Filepath.h"


DeferredShading::DeferredShading(const Window& window) :
	window(window),
	lamp(Filepath::DeferredShader + "ADS/DeferredLamp.vs", Filepath::DeferredShader + "ADS/DeferredLamp.fs"),
	geometryShader(Filepath::DeferredShader + "ADS/GBuffer.vs", Filepath::DeferredShader + "ADS/GBuffer.fs"),
	ssaoLighting(Filepath::DeferredShader + "ADS/DeferredShading.vs", Filepath::DeferredShader + "ADS/SSAOLighting.fs"),
	ssao(Filepath::DeferredShader + "ADS/DeferredShading.vs", Filepath::DeferredShader + "ADS/SSAO.fs"),
	ssaoBlur(Filepath::DeferredShader + "ADS/DeferredShading.vs", Filepath::DeferredShader + "ADS/SSAOBlur.fs")
{
}

DeferredShading::~DeferredShading()
{
}

void DeferredShading::Initialize(Scene & scene)
{
	const Window::Parameters parameters = window.GetWindowParameters();

	//GBuffer setup
	SetupGBuffer(parameters);

	//SSAO buffer setup
	SetupSSAOBuffers(parameters);

	//Generate sample kernel
	std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;
	CreateSSAOKernel(randomFloats, generator);

	//Generate noise Texture
	CreateNoiseTexture(randomFloats, generator);

	//Shader setup
	SetupShaders(scene);

	glViewport(0, 0, parameters.Width, parameters.Height);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void DeferredShading::SetupSSAOBuffers(const Window::Parameters &parameters)
{
	for (int i = 0; i < 2; ++i)
	{
		aoBuffers[i].Generate();
		aoBuffers[i].Bind();
		aoTextures[i] = Texture::CreateEmpty("aoTexture#" + std::to_string(i), parameters.Width, parameters.Height, GL_RED, GL_RGB, GL_FLOAT);
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

void DeferredShading::SetupGBuffer(const Window::Parameters &parameters)
{
	gBuffer.Generate();
	gBuffer.Bind();

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
}

void DeferredShading::SetupShaders(Scene & scene)
{
	glm::mat4 projection = scene.GetCamera().GetProjectionMatrix();

	geometryShader.Use();
	geometryShader.SetInt("diffuse", 0);
	geometryShader.SetInt("specular", 1);
	geometryShader.SetMat4("projection", projection);

	skyboxShader.Use();
	skyboxShader.SetInt("environmentMap", 0);
	skyboxShader.SetMat4("projection", projection);

	lamp.Use();
	lamp.SetMat4("projection", projection);

	ssaoLighting.Use();
	ssaoLighting.SetInt("gPosition", 0);
	ssaoLighting.SetInt("gNormal", 1);
	ssaoLighting.SetInt("gAlbedoSpecular", 2);
	ssaoLighting.SetInt("ssao", 3);
	ssaoLighting.SetFloat("exposure", 1.0f);
	ssaoLighting.SetFloat("gammaCorrection", 2.2f);

	ssao.Use();
	ssao.SetInt("gPosition", 0);
	ssao.SetInt("gNormal", 1);
	ssao.SetInt("noise", 2);
	ssao.SetMat4("projection", projection);

	ssaoBlur.Use();
	ssaoBlur.SetInt("ssaoInput", 0);
}

void DeferredShading::CreateNoiseTexture(std::uniform_real_distribution<GLfloat> &randomFloats, std::default_random_engine &generator)
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

void DeferredShading::CreateSSAOKernel(std::uniform_real_distribution<GLfloat> &randomFloats, std::default_random_engine &generator)
{
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
}

void DeferredShading::Render(Scene & scene)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = scene.GetCamera().GetViewMatrix();
	//Geometry pass
	GeometryPass(view, scene);

	//Generate SSSAO textures
	SSAOTexturePass();

	//Blur ssao texture to remove noise
	BlurPass();

	//Lighting pass
	const std::vector<Light>& lights = scene.GetLights();

	LightingPass(lights, scene);

	GBufferToDefaultFramebuffer();

	//Render lights on top of scene
	RenderLights(view, lights);

	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	scene.GetSkybox().Draw();
	glDepthFunc(GL_LESS);
}

void DeferredShading::RenderLights(const glm::mat4 &view, const std::vector<Light> & lights)
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

void DeferredShading::GBufferToDefaultFramebuffer()
{
	//Copy content of geometry shader to default framebuffer's depth buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.GetBuffer());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
											   // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
											   // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
											   // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format)
	Window::Parameters parameter = window.GetWindowParameters();

	glBlitFramebuffer(0, 0, parameter.Width, parameter.Height, 0, 0, parameter.Width, parameter.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	gBuffer.Unbind();
}

void DeferredShading::LightingPass(const std::vector<Light> & lights, Scene & scene)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ssaoLighting.Use();
	ssaoLighting.SetFloat("ambientStrength", adsParameters.AmbientStrength);
	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		glm::vec3 lightPositionView = glm::vec3(scene.GetCamera().GetViewMatrix() * glm::vec4(lights[i].GetWorldPosition(), 1.0f));
		ssaoLighting.SetVec3("lights[" + std::to_string(i) + "].Position", lightPositionView);
		ssaoLighting.SetVec3("lights[" + std::to_string(i) + "].Color", lights[i].GetColour());

		const Light::Parameters& parameters = lights[i].GetParameters();
		ssaoLighting.SetFloat("lights[" + std::to_string(i) + "].Constant", parameters.Constant);
		ssaoLighting.SetFloat("lights[" + std::to_string(i) + "].Linear", parameters.Linear);
		ssaoLighting.SetFloat("lights[" + std::to_string(i) + "].Quadratic", parameters.Quadratic);
		for (unsigned int j = 0; j < 3; ++j)
		{
			glActiveTexture(GL_TEXTURE0 + j);
			glBindTexture(GL_TEXTURE_2D, gBufferTextures[j].GetID());
		}
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, aoTextures[1].GetID());
	}

	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

void DeferredShading::BlurPass()
{
	aoBuffers[1].Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	ssaoBlur.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aoTextures[0].GetID());
	quad.Render();
	aoBuffers[1].Unbind();
}

void DeferredShading::SSAOTexturePass()
{
	aoBuffers[0].Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	ssao.Use();

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

void DeferredShading::GeometryPass(const glm::mat4 &view, Scene & scene)
{
	gBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
}
