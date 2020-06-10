#include "Rendererpch.h"
#include "ForwardADS.h"

ForwardADS::ForwardADS(Window & window) :
	adsLighting(Shader(Filepath::ForwardShader + "BasicADS.vs", Filepath::ForwardShader + "BasicADS.fs")),
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
	ShadowMapping::Parameters parameters;
	parameters.AspectRatio = 1.0f;
	parameters.Resolution = glm::vec2(1024, 1024);
	shadowMapping.Initialize(parameters);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	MSAA::Parameters msaaParameters;
	msaaParameters.Resolution = glm::ivec2(window.GetWindowParameters().Width, window.GetWindowParameters().Height);
	msaaParameters.Samples = 4;
	msaaParameters.TextureFormat = GL_RGB16F;
	msaa.Initialize(msaaParameters);

	postProcessing = &basic;
	postProcessing->Initialize(window.GetWindowParameters());
	printf("Initialization Complete\n");
}

void ForwardADS::Render(Scene & scene)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	const std::vector<Light>& lights = scene.GetLights();
	const std::vector<Actor>& actors = scene.GetActors();

	shadowMapping.MapPointLights(lights, actors);

	msaa.Bind();
	//postProcessing->Bind();
	Window::Parameters windowParameters = window.GetWindowParameters();
	glViewport(0, 0, windowParameters.Width, windowParameters.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	
	
	SetADSLightingUniforms(scene, lights);

	std::map<float, const Actor*> distanceSortedActors;
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		float distance = glm::length(scene.GetCamera().GetWorldPosition() - actors[i].GetWorldPosition());
		distanceSortedActors[distance] = &actors[i];
	}

	//Render actors
	adsLighting.Use();
	//for (unsigned int i = 0; i < actors.size(); ++i)
	//{
	//	adsLighting.SetMat4("model", actors[i].GetWorldMatrix());
	//	const Material& material = actors[i].GetRenderComponent().GetMaterial();
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Albedo).GetID());
	//	glActiveTexture(GL_TEXTURE1);
	//	glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Metallic).GetID());
	//	actors[i].GetRenderComponent().GetMesh().Draw();
	//}

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
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Metallic).GetID());
		actor->GetRenderComponent().GetMesh().Draw();
	}

	//Render skybox
	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	scene.GetSkybox().Draw();
	glDepthFunc(GL_LESS);

	Framebuffer::BlitParameters blitParameters;
	blitParameters.Destination = &postProcessing->GetFramebuffer();
	blitParameters.Resolution = glm::ivec2(window.GetWindowParameters().Width, window.GetWindowParameters().Height);
	blitParameters.Mask = GL_COLOR_BUFFER_BIT;
	blitParameters.Filter = GL_NEAREST;
	msaa.Blit(blitParameters);
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
	
	for (int i = 0; i < shadowMapping.GetMaximumNumberOfLights(); ++i)
	{
		adsLighting.SetInt("shadowCubeMaps[" + std::to_string(i) + "]", i + 2);
	}
	adsLighting.SetFloat("farPlane", shadowMapping.GetParameters().FarPlane);
	
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
		shadowMapping.BindShadowMap(i);

		std::string lightPosition = std::string("lights[") + std::to_string(i) + std::string("].Position");
		std::string lightColour = std::string("lights[") + std::to_string(i) + std::string("].Colour");
		adsLighting.SetVec3(lightPosition, lights[i].GetWorldPosition());
		adsLighting.SetVec3(lightColour, lights[i].GetColour());
		const Light::Parameters& parameters = lights[i].GetParameters();
		
		lamp.Use();
		lamp.SetMat4("model", lights[i].GetWorldMatrix());
		lamp.SetVec3("lightColour", lights[i].GetColour());
		lights[i].GetRenderComponent().GetMesh().Draw();
	}
	glActiveTexture(GL_TEXTURE0);
}
