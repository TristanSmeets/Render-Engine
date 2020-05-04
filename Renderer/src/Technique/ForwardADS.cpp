#include "Rendererpch.h"
#include "ForwardADS.h"

ForwardADS::ForwardADS(Window & window) :
	adsLighting(Shader(Filepath::ForwardShader + "BasicADS.vs", Filepath::ForwardShader + "BasicADS.fs")),
	pointShadowDepth(Shader(Filepath::ForwardShader + "PointLightDepthMap.vs", Filepath::ForwardShader + "PointLightDepthMap.fs", Filepath::ForwardShader + "PointLightDepthMap.gs")),
	lamp(Shader(Filepath::ForwardShader + "Lamp.vs", Filepath::ForwardShader + "Lamp.fs")),
	window(window)
{
}

ForwardADS::~ForwardADS()
{
}

void ForwardADS::Initialize(Scene & scene)
{
	printf("Initializing ForwardADS\n");

	SetupShaders(scene);
	SetupPointLightBuffer();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	postProcessing = &bloom;
	postProcessing->Initialize(window.GetWindowParameters());
	printf("Initialization Complete\n");
}

void ForwardADS::Render(Scene & scene)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const std::vector<Light>& lights = scene.GetLights();
	const std::vector<Actor>& actors = scene.GetActors();

	CreatePointLightShadows(lights, actors);

	postProcessing->Bind();
	Window::Parameters windowParameters = window.GetWindowParameters();
	glViewport(0, 0, windowParameters.Width, windowParameters.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	SetADSLightingUniforms(scene, lights);

	//Render actors
	adsLighting.Use();
	adsLighting.SetFloat("material.AmbientStrength", adsParameters.AmbientStrength);
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		adsLighting.SetMat4("model", actors[i].GetWorldMatrix());
		const Material& material = actors[i].GetRenderComponent().GetMaterial();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Albedo).GetID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Roughness).GetID());
		actors[i].GetRenderComponent().GetMesh().Draw();
	}

	//Render skybox
	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	scene.GetSkybox().Draw();
	glDepthFunc(GL_LESS);

	postProcessing->Unbind();
	postProcessing->Draw();
}

void ForwardADS::SetupShaders(Scene & scene)
{
	glm::mat4 projection = scene.GetCamera().GetProjectionMatrix();

	skyboxShader.Use();
	skyboxShader.SetInt("environmentMap", 0);
	skyboxShader.SetMat4("projection", projection);

	lamp.Use();
	lamp.SetMat4("projection", projection);

	adsLighting.Use();
	adsLighting.SetMat4("projection", projection);
	adsLighting.SetInt("material.Diffuse", 0);
	adsLighting.SetInt("material.Specular", 1);
	
	for (int i = 0; i < maximumLights; ++i)
	{
		adsLighting.SetInt("shadowCubeMaps[" + std::to_string(i) + "]", i + 2);
	}
	adsLighting.SetFloat("farPlane", farPlane);
	
}

void ForwardADS::SetupPointLightBuffer()
{
	for (int i = 0; i < maximumLights; ++i)
	{
		Cubemap& shadowMap = shadowCubeMaps[i];
		shadowMap.CreateTexture(shadowWidth, shadowHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
		shadowMap.SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		shadowMap.SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	pointDepthBuffer.Generate();
	pointDepthBuffer.Unbind();
}

void ForwardADS::CreatePointLightShadows(const std::vector<Light>& lights, const std::vector<Actor>& actors)
{
	glViewport(0, 0, shadowWidth, shadowHeight);
	pointDepthBuffer.Bind();
	pointShadowDepth.Use();
	pointShadowDepth.SetFloat("farPlane", farPlane);

	for (int i = 0; i < lights.size(); ++i)
	{
		const Cubemap& shadowCubeMap = shadowCubeMaps[i];
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubeMap.GetID(), 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		const Light& light = lights[i];
		const glm::vec3 lightPosition = light.GetWorldPosition();
		glm::mat4 shadowTransforms[6] =
		{
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		for (unsigned int i = 0; i < 6; ++i)
		{
			pointShadowDepth.SetMat4(std::string("shadowMatrices[") + std::to_string(i) + std::string("]"), shadowTransforms[i]);
		}
		pointShadowDepth.SetVec3("lightPosition", lightPosition);

		glCullFace(GL_FRONT);
		for (int i = 0; i < actors.size(); ++i)
		{
			pointShadowDepth.SetMat4("model", actors[i].GetWorldMatrix());
			actors[i].GetRenderComponent().GetMesh().Draw();
		}
		glCullFace(GL_BACK);
		pointShadowDepth.SetMat4("model", actors[actors.size() - 1].GetWorldMatrix());
		actors[actors.size() - 1].GetRenderComponent().GetMesh().Draw();
	}

	pointDepthBuffer.Unbind();
}

void ForwardADS::SetADSLightingUniforms(Scene & scene, const std::vector<Light>& lights)
{
	adsLighting.Use();
	adsLighting.SetMat4("view", scene.GetCamera().GetViewMatrix());
	adsLighting.SetVec3("viewPosition", scene.GetCamera().GetWorldPosition());

	
	lamp.Use();
	lamp.SetMat4("view", scene.GetCamera().GetViewMatrix());

	for (int i = 0; i < lights.size(); ++i)
	{
		adsLighting.Use();
		glActiveTexture(GL_TEXTURE2 + i);
		shadowCubeMaps[i].Bind();

		std::string lightPosition = std::string("lights[") + std::to_string(i) + std::string("].Position");
		std::string lightColour = std::string("lights[") + std::to_string(i) + std::string("].Colour");
		std::string lightConstant = std::string("lights[") + std::to_string(i) + std::string("].Constant");
		std::string lightLinear = std::string("lights[") + std::to_string(i) + std::string("].Linear");
		std::string lightQuadratic = std::string("lights[") + std::to_string(i) + std::string("].Quadratic");
		adsLighting.SetVec3(lightPosition, lights[i].GetWorldPosition());
		adsLighting.SetVec3(lightColour, lights[i].GetColour());
		const Light::Parameters& parameters = lights[i].GetParameters();
		adsLighting.SetFloat(lightConstant, parameters.Constant);
		adsLighting.SetFloat(lightLinear, parameters.Linear);
		adsLighting.SetFloat(lightQuadratic, parameters.Quadratic);
		
		lamp.Use();
		lamp.SetMat4("model", lights[i].GetWorldMatrix());
		lamp.SetVec3("lightColour", lights[i].GetColour());
		lights[i].GetRenderComponent().GetMesh().Draw();
	}
	glActiveTexture(GL_TEXTURE0);
}
