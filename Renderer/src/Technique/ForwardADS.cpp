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

	postProcessing->Bind();
	Window::Parameters windowParameters = window.GetWindowParameters();
	glViewport(0, 0, windowParameters.Width, windowParameters.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	SetADSLightingUniforms(scene, lights);

	//Render actors
	adsLighting.Use();
	adsLighting.SetFloat("material.AmbientStrength", adsParameters.AmbientStrength);
	adsLighting.SetFloat("material.Shininess", adsParameters.Shininess);
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		adsLighting.SetMat4("model", actors[i].GetWorldMatrix());
		const Material& material = actors[i].GetRenderComponent().GetMaterial();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Albedo).GetID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Metallic).GetID());
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
