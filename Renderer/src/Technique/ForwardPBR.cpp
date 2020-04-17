#include "Rendererpch.h"
#include "ForwardPBR.h"
#include "Utility/Filepath.h"

ForwardPBR::ForwardPBR(Window& window) : RenderTechnique(),
pbr(Shader(Filepath::Shader + "PBR.vs", Filepath::Shader + "PBR.fs")),
directionalShadowDepth(Shader(Filepath::Shader + "ShadowDepthMap.vs", Filepath::Shader + "ShadowDepthMap.fs")),
pointShadowDepth(Shader(Filepath::Shader + "PointLightDepthMap.vs", Filepath::Shader + "PointLightDepthMap.fs", Filepath::Shader + "PointLightDepthMap.gs")),
lamp(Shader(Filepath::Shader + "Lamp.vs", Filepath::Shader + "Lamp.fs")),
window(window)
{
}

ForwardPBR::~ForwardPBR()
{
}

void ForwardPBR::Initialize(Scene & scene)
{
	printf("Initializing ForwardPBR\n");


	glm::mat4 projection = scene.GetCamera().GetProjectionMatrix();
	const Skybox& skybox = scene.GetSkybox();

	//Shader setup
	pbr.Use();
	pbr.SetMat4("projection", projection);
	pbr.SetInt("irradianceMap", 5);
	pbr.SetInt("prefilterMap", 6);
	pbr.SetInt("brdfLUT", 7);
	pbr.SetInt("shadowMap", 8);
	pbr.SetInt("shadowCubeMap", 9);

	skyboxShader.Use();
	skyboxShader.SetInt("environmentMap", 0);
	skyboxShader.SetMat4("projection", projection);

	lamp.Use();
	lamp.SetMat4("projection", projection);

	//Texture depth map.
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

	//Cubemap depth map
	pointDepthBuffer.Generate();
	pointShadowsDepthMap.CreateTexture(shadowWidth, shadowHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
	pointShadowsDepthMap.SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	pointShadowsDepthMap.SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	pointDepthBuffer.Bind();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointShadowsDepthMap.GetID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	pointDepthBuffer.Unbind();

	//Setup depth testing and culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	postProcessing.Initialize(window.GetWindowParameters());
	printf("Initializion Complete\n\n");
}

void ForwardPBR::Render(Scene & scene)
{
	glClearColor(2.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Directional light shadow
	glViewport(0, 0, shadowWidth, shadowHeight);
	directionalDepthBuffer.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	const std::vector<Light>& lights = scene.GetLights();

	glm::vec3 lightDirection = scene.GetDirectionalLight().GetFront();
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 20.0f);
	glm::mat4 lightview = scene.GetDirectionalLight().GetView();

	glm::mat4 lightSpaceMatrix = lightProjection * lightview;
	directionalShadowDepth.Use();
	directionalShadowDepth.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	const std::vector<Actor>& actors = scene.GetActors();
	//glCullFace(GL_FRONT);
	for (int i = 0; i < actors.size(); ++i)
	{
		directionalShadowDepth.SetMat4("model", actors[i].GetWorldMatrix());
		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	//glCullFace(GL_BACK);
	directionalShadowDepth.SetMat4("model", actors[actors.size() - 1].GetWorldMatrix());
	actors[actors.size() - 1].GetRenderComponent().GetMesh().Draw();
	directionalDepthBuffer.Unbind();

	//Point light shadow
	float aspect = (float)shadowHeight / (float)shadowHeight;
	float nearPlane = 1.0f;
	float farPlane = 25.0f;
	glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);

	glm::vec3 lightPosition = lights[0].GetWorldPosition();
	glm::mat4 shadowTransforms[6] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	glViewport(0, 0, shadowWidth, shadowHeight);
	pointDepthBuffer.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	pointShadowDepth.Use();
	for (unsigned int i = 0; i < 6; ++i)
	{
		pointShadowDepth.SetMat4(std::string("shadowMatrices[") + std::to_string(i) + std::string("]"), shadowTransforms[i]);
	}
	pointShadowDepth.SetVec3("lightPosition", lightPosition);
	pointShadowDepth.SetFloat("farPlane", farPlane);

	glCullFace(GL_FRONT);
	for (int i = 0; i < actors.size(); ++i)
	{
		pointShadowDepth.SetMat4("model", actors[i].GetWorldMatrix());
		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	glCullFace(GL_BACK);
	pointShadowDepth.SetMat4("model", actors[actors.size() - 1].GetWorldMatrix());
	actors[actors.size() - 1].GetRenderComponent().GetMesh().Draw();
	pointDepthBuffer.Unbind();

	//Render the scene as normal with shadow mapping(using depth map)
	postProcessing.Bind();
	Window::Parameters windowParameters = window.GetWindowParameters();
	glViewport(0, 0, windowParameters.Width, windowParameters.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Rendering the Actors
	pbr.Use();
	pbr.SetMat4("view", scene.GetCamera().GetViewMatrix());
	pbr.SetVec3("cameraPos", scene.GetCamera().GetWorldPosition());
	//pbr.SetVec3("cameraPos", lightDirection);
	pbr.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	pbr.SetVec3("lightDirection", lightDirection);
	pbr.SetVec3("lightPosition", lightPosition);
	pbr.SetFloat("farPlane", farPlane);

	const Skybox& skybox = scene.GetSkybox();
	glActiveTexture(GL_TEXTURE5);
	skybox.GetIrradiance().Bind();
	glActiveTexture(GL_TEXTURE6);
	skybox.GetPrefilter().Bind();
	skybox.GetLookup().Bind(pbr, (Texture::Type)7);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, shadow.GetID());
	glActiveTexture(GL_TEXTURE9);
	pointShadowsDepthMap.Bind();
	glActiveTexture(GL_TEXTURE0);

	//Set Lights
	lamp.Use();
	lamp.SetMat4("view", scene.GetCamera().GetViewMatrix());
	lamp.SetMat4("model", scene.GetDirectionalLight().GetWorldMatrix());
	scene.GetDirectionalLight().GetRenderComponent().GetMesh().Draw();
	
	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		lamp.Use();
		lamp.SetMat4("model", lights[i].GetWorldMatrix());
		lights[i].GetRenderComponent().GetMesh().Draw();
		std::string lightPosition = std::string("lightPositions[") + std::to_string(i) + std::string("]");
		std::string lightColour = std::string("lightColours[") + std::to_string(i) + std::string("]");
		pbr.Use();
		pbr.SetVec3(lightPosition, lights[i].GetWorldPosition());
		pbr.SetVec3(lightColour, lights[i].GetColour());
	}


	//Render actors
	//glCullFace(GL_FRONT);
	pbr.Use();
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		pbr.SetMat4("model", actors[i].GetWorldMatrix());
		const Material& material = actors[i].GetRenderComponent().GetMaterial();
		material.GetTexture(Texture::Albedo).Bind(pbr, Texture::Albedo);
		material.GetTexture(Texture::Normal).Bind(pbr, Texture::Normal);
		material.GetTexture(Texture::Metallic).Bind(pbr, Texture::Metallic);
		material.GetTexture(Texture::Roughness).Bind(pbr, Texture::Roughness);
		material.GetTexture(Texture::AmbientOcclusion).Bind(pbr, Texture::AmbientOcclusion);
		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	//glCullFace(GL_BACK);
	//Render skybox
	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	//glActiveTexture(GL_TEXTURE0);
	//pointShadowsDepthMap.Bind();
	skybox.Draw();
	glDepthFunc(GL_LESS);
	postProcessing.Unbind();
	Shader& shader = postProcessing.GetShader();
	shader.Use();
	shader.SetInt("shadowTexture", 1);
	shadow.Bind(shader, Texture::Normal);
	postProcessing.Draw();
}
