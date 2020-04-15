#include "Rendererpch.h"
#include "ForwardPBR.h"
#include "Utility/Filepath.h"

ForwardPBR::ForwardPBR(Window& window) : RenderTechnique(),
pbr(Shader(Filepath::Shader + "PBR.vs", Filepath::Shader + "PBR.fs")),
shadowDepth(Shader(Filepath::Shader + "ShadowDepthMap.vs", Filepath::Shader + "ShadowDepthMap.fs")),
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

	pbr.Use();
	pbr.SetMat4("projection", projection);
	pbr.SetInt("irradianceMap", 5);
	pbr.SetInt("prefilterMap", 6);
	pbr.SetInt("brdfLUT", 7);
	pbr.SetInt("shadowMap", 8);

	skyboxShader.Use();
	skyboxShader.SetInt("environmentMap", 0);
	skyboxShader.SetMat4("projection", projection);

	depthBuffer.Generate();
	shadowTexture = Texture::CreateEmpty("ShadowDepth", shadowWidth, shadowHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	depthBuffer.Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture.GetID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	depthBuffer.Unbind();

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

	glViewport(0, 0, shadowWidth, shadowHeight);
	depthBuffer.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	const std::vector<Light>& lights = scene.GetLights();

	glm::vec3 lightDirection = lights[lights.size() - 1].GetTransform().GetRotation();
	glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, scene.GetCamera().GetFrustum().NearPlaneCutoff, scene.GetCamera().GetFrustum().FarPlaneCutoff);
	glm::mat4 lightview = glm::lookAt
	(
		lightDirection,
		glm::vec3(0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	glm::mat4 lightSpaceMatrix = lightProjection * lightview;
	shadowDepth.Use();
	shadowDepth.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	const std::vector<Actor>& actors = scene.GetActors();
	glCullFace(GL_FRONT);
	for (int i = 0; i < actors.size(); ++i)
	{
		shadowDepth.SetMat4("model", actors[i].GetWorldMatrix());
		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	glCullFace(GL_BACK);
	actors[actors.size() - 1].GetRenderComponent().GetMesh().Draw();
	depthBuffer.Unbind();
	postProcessing.Bind();
	//Render the scene as normal with shadow mapping(using depth map)
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
	
	const Skybox& skybox = scene.GetSkybox();
	glActiveTexture(GL_TEXTURE5);
	skybox.GetIrradiance().Bind();
	glActiveTexture(GL_TEXTURE6);
	skybox.GetPrefilter().Bind();
	skybox.GetLookup().Bind(pbr, (Texture::Type)7);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, shadowTexture.GetID());

	glActiveTexture(GL_TEXTURE0);
	
	//Set Lights
	for (unsigned int i = 0; i < lights.size()- 1; ++i)
	{
		std::string lightPosition = std::string("lightPositions[") + std::to_string(i) + std::string("]");
		std::string lightColour = std::string("lightColours[") + std::to_string(i) + std::string("]");
		pbr.SetVec3(lightPosition, lights[i].GetWorldPosition());
		pbr.SetVec3(lightColour, lights[i].GetColour());
	}

	//Render actors
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

	//Render skybox
	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	skybox.Draw();
	glDepthFunc(GL_LESS);
	postProcessing.Unbind();
	Shader& shader = postProcessing.GetShader();
	shader.Use();
	shader.SetInt("shadowTexture", 1);
	shadowTexture.Bind(shader, Texture::Normal);
	postProcessing.Draw();
}
