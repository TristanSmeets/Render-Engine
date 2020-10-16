#include "Rendererpch.h"
#include "ForwardPBR.h"
#include "Utility/Filepath.h"

ForwardPBR::ForwardPBR(Window& window) : RenderTechnique(),
pbr(Shader(Filepath::ForwardShader + "PBR.vert.glsl", Filepath::ForwardShader + "PBR.frag.glsl")),
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
	
	ShadowMapping::Parameters parameters;
	shadowMapping.Initialize(parameters);

	//Setup depth testing and culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//MSAA::Parameters msaaParameters;
	//msaaParameters.Resolution = glm::ivec2(window.GetWindowParameters().Width, window.GetWindowParameters().Height);
	//msaaParameters.Samples = 8;
	//msaaParameters.TextureFormat = GL_RGB16F;
	//msaa.Initialize(msaaParameters);

	postProcessing = &basic;
	postProcessing->Initialize(window.GetWindowParameters());
	printf("Initializion Complete\n");
}

void ForwardPBR::SetupShaders(Scene & scene)
{
	glm::mat4 projection = scene.GetCamera().GetProjectionMatrix();
	const Skybox& skybox = scene.GetSkybox();

	pbr.Use();
	//pbr.SetMat4("ProjectionMatrix", projection);
	pbr.SetInt("Material.Albedo", 0);
	pbr.SetInt("Material.Normal", 1);
	pbr.SetInt("Material.MRAO", 2);

	skyboxShader.Use();
	skyboxShader.SetInt("environmentMap", 0);
	skyboxShader.SetMat4("projection", projection);

	lamp.Use();
	lamp.SetMat4("projection", projection);
}

void ForwardPBR::Render(Scene & scene)
{
	glClearColor(2.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const std::vector<Light>& lights = scene.GetLights();
	const std::vector<Actor>& actors = scene.GetActors();
	
	shadowMapping.MapPointLights(lights, actors);

	//Render the scene as normal with shadow mapping(using depth map)
	
	//msaa.Bind();
	postProcessing->Bind();
	Window::Parameters windowParameters = window.GetWindowParameters();
	glViewport(0, 0, windowParameters.Width, windowParameters.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	const Skybox& skybox = scene.GetSkybox();

	SetPBRShaderUniforms(scene, skybox, lights);

	std::map<float, const Actor*> distanceSortedActors;
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		if (!actors[i].GetRenderComponent().GetPBRParameters().IsTransparent)
		{
			continue;
		}
		float distance = glm::length(scene.GetCamera().GetWorldPosition() - actors[i].GetWorldPosition());
		distanceSortedActors[distance] = &actors[i];
	}

	//Render actors
	pbr.Use();
	glm::mat4 p = scene.GetCamera().GetProjectionMatrix();
	pbr.SetMat4("ProjectionMatrix", p);
	
	for (unsigned int i = 0; i < actors.size(); ++i)
	{
		glm::mat4 vm = scene.GetCamera().GetViewMatrix() * actors[i].GetWorldMatrix();
		pbr.SetMat4("ModelViewMatrix", vm);
		pbr.SetMat3("NormalMatrix", glm::mat3(vm));
		pbr.SetMat4("MVP", p * vm);

		const Material& material = actors[i].GetRenderComponent().GetMaterial();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Albedo).GetID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::Normal).GetID());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material.GetTexture(Texture::MRAO).GetID());
		glActiveTexture(GL_TEXTURE0);
		actors[i].GetRenderComponent().GetMesh().Draw();
	}
	
	//Render skybox
	glDepthFunc(GL_LEQUAL);
	skyboxShader.Use();
	skyboxShader.SetMat4("view", scene.GetCamera().GetViewMatrix());
	skybox.Draw();
	glDepthFunc(GL_LESS);

	/*Framebuffer::BlitParameters blitParameters;
	blitParameters.Destination = &postProcessing->GetFramebuffer();
	blitParameters.Resolution = glm::ivec2(window.GetWindowParameters().Width, window.GetWindowParameters().Height);
	blitParameters.Mask = GL_COLOR_BUFFER_BIT;
	blitParameters.Filter = GL_NEAREST;
	msaa.Blit(blitParameters);*/
	postProcessing->Unbind();
	postProcessing->Apply();
	postProcessing->Draw();
}

void ForwardPBR::SetPBRShaderUniforms(Scene & scene, const Skybox & skybox, const std::vector<Light> & lights)
{
	pbr.Use();
	pbr.SetInt("NumberOfLights", scene.GetNumberOfLights());
	

	//Set Lights
	lamp.Use();
	glm::mat4 view = scene.GetCamera().GetViewMatrix();
	lamp.SetMat4("view", view);

	for (unsigned int i = 0; i < scene.GetNumberOfLights(); ++i)
	{
		lamp.Use();
		lamp.SetMat4("model", lights[i].GetWorldMatrix());
		lamp.SetVec3("lightColour", lights[i].GetColour());
		lights[i].GetRenderComponent().GetMesh().Draw();
		std::string lightPosition = std::string("Lights[") + std::to_string(i) + std::string("].Position");
		std::string lightIntensity = std::string("Lights[") + std::to_string(i) + std::string("].Intensity");
		pbr.Use();
		pbr.SetVec4(lightPosition, view * glm::vec4(lights[i].GetWorldPosition(), 1.0f));
		pbr.SetVec3(lightIntensity, lights[i].GetColour());
	}
}
