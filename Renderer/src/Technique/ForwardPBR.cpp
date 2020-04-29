#include "Rendererpch.h"
#include "ForwardPBR.h"
#include "Utility/Filepath.h"

ForwardPBR::ForwardPBR(Window& window) : RenderTechnique(),
pbr(Shader(Filepath::ForwardShader + "PBR.vs", Filepath::ForwardShader + "PBR.fs")),
//directionalShadowDepth(Shader(Filepath::ForwardShader + "ShadowDepthMap.vs", Filepath::ForwardShader + "ShadowDepthMap.fs")),
pointShadowDepth(Shader(Filepath::ForwardShader + "PointLightDepthMap.vs", Filepath::ForwardShader + "PointLightDepthMap.fs", Filepath::ForwardShader + "PointLightDepthMap.gs")),
lamp(Shader(Filepath::ForwardShader + "Lamp.vs", Filepath::ForwardShader + "Lamp.fs")),
window(window)
{
}

ForwardPBR::~ForwardPBR()
{
}

void ForwardPBR::Initialize(Scene & scene)
{
	printf("Initializing ForwardPBR\n");

	SetupShaders(scene);	
	SetupDirectionalShadowBuffer();
	SetupPointLightBuffer();

	//Setup depth testing and culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	postProcessing = &basic;
	postProcessing->Initialize(window.GetWindowParameters());
	printf("Initializion Complete\n\n");
}

void ForwardPBR::SetupShaders(Scene & scene)
{
	glm::mat4 projection = scene.GetCamera().GetProjectionMatrix();
	const Skybox& skybox = scene.GetSkybox();

	pbr.Use();
	pbr.SetMat4("projection", projection);
	pbr.SetInt("irradianceMap", 5);
	pbr.SetInt("prefilterMap", 6);
	pbr.SetInt("brdfLUT", 7);
	pbr.SetInt("shadowMap", 8);

	for (int i = 0; i < maximumLights; ++i)
	{
		pbr.SetInt("shadowCubeMaps[" + std::to_string(i) + "]", i + 9);
	}

	skyboxShader.Use();
	skyboxShader.SetInt("environmentMap", 0);
	skyboxShader.SetMat4("projection", projection);

	lamp.Use();
	lamp.SetMat4("projection", projection);
}

void ForwardPBR::SetupPointLightBuffer()
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

void ForwardPBR::SetupDirectionalShadowBuffer()
{
	/*
	directionalDepthBuffer.Generate();
	shadow = Texture::CreateEmpty("ShadowDepth", shadowWidth, shadowHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	
	directionalDepthBuffer.Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow.GetID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	directionalDepthBuffer.Unbind();
	*/
}

void ForwardPBR::Render(Scene & scene)
{
	glClearColor(2.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const std::vector<Light>& lights = scene.GetLights();
	const std::vector<Actor>& actors = scene.GetActors();
	
	//CreateDirectionalLightShadow(scene, actors);
	
	CreatePointLightShadows(lights, actors);

	//Render the scene as normal with shadow mapping(using depth map)
	postProcessing->Bind();
	Window::Parameters windowParameters = window.GetWindowParameters();
	glViewport(0, 0, windowParameters.Width, windowParameters.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	const Skybox& skybox = scene.GetSkybox();

	SetPBRShaderUniforms(scene, skybox, lights);

	//Render actors
	pbr.Use();
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		pbr.SetMat4("model", actors[i].GetWorldMatrix());
		pbr.SetVec3("NonMetallicReflectionColour", glm::vec3(0.04f));
		const Material& material = actors[i].GetRenderComponent().GetMaterial();
		material.GetTexture(Texture::Albedo).Bind(pbr, Texture::Albedo);
		material.GetTexture(Texture::Normal).Bind(pbr, Texture::Normal);
		material.GetTexture(Texture::Metallic).Bind(pbr, Texture::Metallic);
		material.GetTexture(Texture::Roughness).Bind(pbr, Texture::Roughness);
		material.GetTexture(Texture::AmbientOcclusion).Bind(pbr, Texture::AmbientOcclusion);
		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	//Render skybox
	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	skybox.Draw();
	glDepthFunc(GL_LESS);

	postProcessing->Unbind();
	postProcessing->Draw();
}

void ForwardPBR::SetPBRShaderUniforms(Scene & scene, const Skybox & skybox, const std::vector<Light> & lights)
{
	pbr.Use();
	pbr.SetMat4("view", scene.GetCamera().GetViewMatrix());
	pbr.SetVec3("cameraPos", scene.GetCamera().GetWorldPosition());
	pbr.SetFloat("farPlane", farPlane);
	pbr.SetVec3("viewpos", scene.GetCamera().GetWorldPosition());

	glActiveTexture(GL_TEXTURE5);
	skybox.GetIrradiance().Bind();
	glActiveTexture(GL_TEXTURE6);
	skybox.GetPrefilter().Bind();
	skybox.GetLookup().Bind(pbr, (Texture::Type)7);
	//glActiveTexture(GL_TEXTURE8);
	//glBindTexture(GL_TEXTURE_2D, shadow.GetID());
	for (int i = 0; i < lights.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE9 + i);
		shadowCubeMaps[i].Bind();
	}
	glActiveTexture(GL_TEXTURE0);

	//Set Lights
	lamp.Use();
	lamp.SetMat4("view", scene.GetCamera().GetViewMatrix());
	//lamp.SetMat4("model", scene.GetDirectionalLight().GetWorldMatrix());
	//scene.GetDirectionalLight().GetRenderComponent().GetMesh().Draw();

	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		lamp.Use();
		lamp.SetMat4("model", lights[i].GetWorldMatrix());
		lamp.SetVec3("lightColour", lights[i].GetColour());
		lights[i].GetRenderComponent().GetMesh().Draw();
		std::string lightPosition = std::string("lights[") + std::to_string(i) + std::string("].Position");
		std::string lightColour = std::string("lights[") + std::to_string(i) + std::string("].Colour");
		std::string lightConstant = std::string("lights[") + std::to_string(i) + std::string("].Constant");
		std::string lightLinear = std::string("lights[") + std::to_string(i) + std::string("].Linear");
		std::string lightQuadratic = std::string("lights[") + std::to_string(i) + std::string("].Quadratic");
		pbr.Use();
		pbr.SetVec3(lightPosition, lights[i].GetWorldPosition());
		pbr.SetVec3(lightColour, lights[i].GetColour());
		pbr.SetFloat(lightConstant, 1.0f);
		pbr.SetFloat(lightLinear, 0.14f);
		pbr.SetFloat(lightQuadratic, .07f);
	}
}

void ForwardPBR::CreatePointLightShadows(const std::vector<Light> & lights, const std::vector<Actor> & actors)
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

void ForwardPBR::CreateDirectionalLightShadow(Scene & scene, const std::vector<Actor> & actors)
{
	/*
	glViewport(0, 0, shadowWidth, shadowHeight);
	directionalDepthBuffer.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	
	
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 100.0f);
	glm::mat4 lightview = glm::lookAt(scene.GetDirectionalLight().GetWorldPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 lightview = scene.GetDirectionalLight().GetView();
	
	glm::mat4 lightSpaceMatrix = lightProjection * lightview;
	directionalShadowDepth.Use();
	directionalShadowDepth.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	
	glCullFace(GL_FRONT);
	for (int i = 0; i < actors.size(); ++i)
	{
		directionalShadowDepth.SetMat4("model", actors[i].GetWorldMatrix());
		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	glCullFace(GL_BACK);
	directionalShadowDepth.SetMat4("model", actors[actors.size() - 1].GetWorldMatrix());
	actors[actors.size() - 1].GetRenderComponent().GetMesh().Draw();
	directionalDepthBuffer.Unbind();
	*/
}
