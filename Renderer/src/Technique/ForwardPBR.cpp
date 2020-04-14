#include "Rendererpch.h"
#include "ForwardPBR.h"
#include "Utility/Filepath.h"

ForwardPBR::ForwardPBR(Window& window) : RenderTechnique(),
pbr(Shader(Filepath::Shader + "PBR.vs", Filepath::Shader + "PBR.fs")),
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

	skyboxShader.Use();
	skyboxShader.SetInt("environmentMap", 0);
	skyboxShader.SetMat4("projection", projection);

	//Setup depth testing and culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Configure viewport to original framebuffer
	Window::Parameters windowParameters = window.GetWindowParameters();
	glViewport(0, 0, windowParameters.Width, windowParameters.Height);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	printf("Initializion Complete\n\n");
}

void ForwardPBR::Render(Scene & scene)
{
	glClearColor(2.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Rendering the Actors
	pbr.Use();
	pbr.SetMat4("view", scene.GetCamera().GetViewMatrix());
	pbr.SetVec3("cameraPos", scene.GetCamera().GetWorldPosition());

	const std::vector<Actor>& actors = scene.GetActors();
	const std::vector<Light>& lights = scene.GetLights();

	
	const Skybox& skybox = scene.GetSkybox();
	glActiveTexture(GL_TEXTURE5);
	skybox.GetIrradiance().Bind();
	glActiveTexture(GL_TEXTURE6);
	skybox.GetPrefilter().Bind();
	skybox.GetLookup().Bind(pbr, (Texture::Type)7);
	glActiveTexture(GL_TEXTURE0);
	
	//Set Lights
	for (unsigned int i = 0; i < lights.size(); ++i)
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
}
