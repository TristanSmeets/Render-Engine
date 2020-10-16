#include "Rendererpch.h"
#include "DeferredADS.h"
#include "Utility/Filepath.h"


DeferredADS::DeferredADS(const Window& window) :
	window(window),
	lamp(Filepath::DeferredShader + "ADS/DeferredLamp.vs", Filepath::DeferredShader + "ADS/DeferredLamp.fs"),
	geometryShader(Filepath::DeferredShader + "ADS/GBuffer.vs", Filepath::DeferredShader + "ADS/GBuffer.fs"),
	ssaoLighting(Filepath::DeferredShader + "ADS/DeferredShading.vs", Filepath::DeferredShader + "ADS/SSAOLighting.fs"),
	ssao(Filepath::DeferredShader + "ADS/DeferredShading.vs", Filepath::DeferredShader + "ADS/SSAO.fs"),
	ssaoBlur(Filepath::DeferredShader + "ADS/DeferredShading.vs", Filepath::DeferredShader + "ADS/SSAOBlur.fs"),
	adsLighting(Shader(Filepath::ForwardShader + "BasicADS.vs", Filepath::ForwardShader + "BasicADS.fs"))
{
}

DeferredADS::~DeferredADS()
{
}

void DeferredADS::Initialize(Scene & scene)
{
	const Window::Parameters parameters = window.GetWindowParameters();

	//GBuffer setup
	SetupGBuffer(parameters);

	//SSAO buffer setup
	SetupSSAOBuffers(parameters);

	ShadowMapping::Parameters shadowParameters;
	shadowParameters.AspectRatio = 1.0f;
	shadowParameters.Resolution = glm::vec2(1024, 1024);
	shadowMapping.Initialize(shadowParameters);

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

	FXAA::Parameters fxaaParameters = deferredParameters.FxaaParameters;
	fxaaParameters.Resolution = glm::ivec2(parameters.Width, parameters.Height);
	fxaa.Initialize(fxaaParameters);

	postProcessing = &bloom;
	postProcessing->Initialize(parameters);
}

void DeferredADS::SetupSSAOBuffers(const Window::Parameters &parameters)
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

void DeferredADS::SetupGBuffer(const Window::Parameters &parameters)
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
	//View positions
	gBufferTextures[3] = Texture::CreateEmpty("ViewPositions", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
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

void DeferredADS::SetupShaders(Scene & scene)
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
	ssaoLighting.SetFloat("aspectRatio", scene.GetCamera().GetFrustum().AspectRatio);
	float tanHalfFOV = tanf(glm::radians(scene.GetCamera().GetFrustum().FieldOfView / 2.0f));
	ssaoLighting.SetFloat("tanHalfFOV", tanHalfFOV);
	ssaoLighting.SetMat4("projection", projection);
	ssaoLighting.SetInt("gPosition", 0);
	ssaoLighting.SetInt("gNormal", 1);
	ssaoLighting.SetInt("gAlbedoSpecular", 2);
	ssaoLighting.SetInt("ssao", 3);
	for (int i = 0; i < shadowMapping.GetMaximumNumberOfLights(); ++i)
	{
		ssaoLighting.SetInt("shadowCubeMaps[" + std::to_string(i) + "]", i + 4);
	}

	ssao.Use();
	ssao.SetInt("gPosition", 0);
	ssao.SetInt("gNormal", 1);
	ssao.SetInt("noise", 2);
	ssao.SetMat4("projection", projection);

	ssaoBlur.Use();
	ssaoBlur.SetInt("ssaoInput", 0);

	adsLighting.Use();
	adsLighting.SetMat4("projection", projection);
	adsLighting.SetInt("material.Diffuse", 0);
	adsLighting.SetInt("material.Specular", 1);

	for (int i = 0; i < shadowMapping.GetMaximumNumberOfLights(); ++i)
	{
		adsLighting.SetInt("shadowCubeMaps[" + std::to_string(i) + "]", i + 2);
	}
	adsLighting.SetFloat("farPlane", shadowMapping.GetParameters().FarPlane);
}

void DeferredADS::CreateNoiseTexture(std::uniform_real_distribution<GLfloat> &randomFloats, std::default_random_engine &generator)
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

void DeferredADS::CreateSSAOKernel(std::uniform_real_distribution<GLfloat> &randomFloats, std::default_random_engine &generator)
{
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		//Scale samples so they're more aligned to the center of the kernel
		scale = 0.1f + (scale * scale) * 0.9f;
		sample *= scale;
		ssaoKernel[i] = sample;
	}
}

void DeferredADS::Render(Scene & scene)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shadowMapping.MapPointLights(scene.GetLights(), scene.GetActors());

	glViewport(0, 0, window.GetWindowParameters().Width, window.GetWindowParameters().Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glm::mat4 view = scene.GetCamera().GetViewMatrix();
	//Geometry pass
	GeometryPass(view, scene);

	//Generate SSSAO textures
	SSAOTexturePass();

	//Blur ssao texture to remove noise
	BlurPass();

	Framebuffer::BlitParameters blitParameters;
	blitParameters.Destination = &postProcessing->GetFramebuffer();
	blitParameters.Resolution = glm::ivec2(window.GetWindowParameters().Width, window.GetWindowParameters().Height);
	blitParameters.Mask = GL_DEPTH_BUFFER_BIT;
	blitParameters.Filter = GL_NEAREST;
	gBuffer.BlitFramebuffer(blitParameters);

	//Lighting pass
	const std::vector<Light>& lights = scene.GetLights();
	LightingPass(lights, scene);
	
	//Render lights on top of scene
	RenderLights(view, lights);
	RenderTransparentActors(view, scene);

	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	scene.GetSkybox().Draw();
	glDepthFunc(GL_LESS);

	postProcessing->Apply();
	fxaa.Bind();
	postProcessing->Draw();
	fxaa.Unbind();
	fxaa.Apply(deferredParameters.FxaaParameters);
}

void DeferredADS::RenderLights(const glm::mat4 &view, const std::vector<Light> & lights)
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

void DeferredADS::RenderTransparentActors(const glm::mat4 & view, Scene& scene)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::vector<Actor>& actors = scene.GetActors();

	std::map<float, const Actor*> distanceSortedActors;
	for (int i = 0; i < actors.size(); ++i)
	{
		if (!actors[i].GetRenderComponent().GetADSParameters().IsTransparent)
		{
			continue;
		}
		float distance = glm::length(scene.GetCamera().GetWorldPosition() - actors[i].GetWorldPosition());
		distanceSortedActors[distance] = &actors[i];
	}

	SetADSLightingUniforms(view, scene.GetCamera().GetWorldPosition(), scene.GetLights());

	for (std::map<float, const Actor*>::reverse_iterator it = distanceSortedActors.rbegin(); it != distanceSortedActors.rend(); ++it)
	{
		const Actor* actor = it->second;
		adsLighting.SetFloat("material.Shininess", actor->GetRenderComponent().GetADSParameters().Shininess);
		adsLighting.SetFloat("material.AmbientStrength", actor->GetRenderComponent().GetADSParameters().AmbientStrength);
		adsLighting.SetMat4("model", actor->GetWorldMatrix());
		const Material& material = actor->GetRenderComponent().GetMaterial();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Albedo).GetID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::MRAO).GetID());
		actor->GetRenderComponent().GetMesh().Draw();
	}
	glDisable(GL_BLEND);
}

void DeferredADS::SetADSLightingUniforms(const glm::mat4& view, const glm::vec3& viewPosition, const std::vector<Light>& lights)
{
	adsLighting.Use();
	adsLighting.SetMat4("view", view);
	adsLighting.SetVec3("viewPosition", viewPosition);

	for (int i = 0; i < lights.size(); ++i)
	{
		adsLighting.Use();
		glActiveTexture(GL_TEXTURE2 + i);
		shadowMapping.BindShadowMap(i);

		std::string lightPosition = std::string("lights[") + std::to_string(i) + std::string("].Position");
		std::string lightColour = std::string("lights[") + std::to_string(i) + std::string("].Colour");
		adsLighting.SetVec3(lightPosition, lights[i].GetWorldPosition());
		adsLighting.SetVec3(lightColour, lights[i].GetColour());
		const Light::Parameters& parameters = lights[i].GetParameters();
	}
	glActiveTexture(GL_TEXTURE0);
}

void DeferredADS::LightingPass(const std::vector<Light> & lights, Scene & scene)
{
	glClear(GL_COLOR_BUFFER_BIT);

	ssaoLighting.Use();
	ssaoLighting.SetFloat("ambientStrength", deferredParameters.AdsParameters.AmbientStrength);
	ssaoLighting.SetFloat("shininess", deferredParameters.AdsParameters.Shininess);
	ssaoLighting.SetVec3("viewPosition", scene.GetCamera().GetWorldPosition());
	ssaoLighting.SetFloat("farPlane", scene.GetCamera().GetFrustum().FarPlaneCutoff);
	ssaoLighting.SetFloat("nearPlane", scene.GetCamera().GetFrustum().NearPlaneCutoff);

	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		ssaoLighting.SetVec3("lights[" + std::to_string(i) + "].Position", lights[i].GetWorldPosition());
		ssaoLighting.SetVec3("lights[" + std::to_string(i) + "].Color", lights[i].GetColour());

		for (unsigned int j = 0; j < 3; ++j)
		{
			glActiveTexture(GL_TEXTURE0 + j);
			glBindTexture(GL_TEXTURE_2D, gBufferTextures[j].GetID());
		}

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, aoTextures[1].GetID());
		glActiveTexture(GL_TEXTURE4 + i);
		shadowMapping.BindShadowMap(i);
	}
	glActiveTexture(GL_TEXTURE0);

	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

void DeferredADS::BlurPass()
{
	aoBuffers[1].Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	ssaoBlur.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aoTextures[0].GetID());
	quad.Render();
	aoBuffers[1].Unbind();
}

void DeferredADS::SSAOTexturePass()
{
	aoBuffers[0].Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	/*ssao.Use();
	ssao.SetInt("occlusionPower", deferredParameters.OcclusionPower);
	ssao.SetInt("kernelSize", deferredParameters.KernelSize);
	ssao.SetFloat("radius", deferredParameters.Radius);
	ssao.SetFloat("bias", deferredParameters.Bias);*/

	for (unsigned int i = 0; i < 64; ++i)
	{
		ssao.SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
	}
	for (unsigned int i = 0; i < 2; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[i + 3].GetID());
	}
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noise.GetID());
	quad.Render();
	aoBuffers[0].Unbind();
}

void DeferredADS::GeometryPass(const glm::mat4 &view, Scene & scene)
{
	gBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	geometryShader.Use();
	geometryShader.SetMat4("view", view);

	const std::vector<Actor>& actors = scene.GetActors();

	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		if (actors[i].GetRenderComponent().GetADSParameters().IsTransparent)
		{
			continue;
		}
		geometryShader.SetMat4("model", actors[i].GetWorldMatrix());
		const Material& material = actors[i].GetRenderComponent().GetMaterial();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Albedo).GetID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::MRAO).GetID());
		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	gBuffer.Unbind();
}
