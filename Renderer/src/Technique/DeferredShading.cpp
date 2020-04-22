#include "Rendererpch.h"
#include "DeferredShading.h"
#include "Utility/Filepath.h"

DeferredShading::DeferredShading(const Window& window) :
	window(window),
	lamp(Filepath::DeferredShader + "DeferredLamp.vs", Filepath::DeferredShader + "DeferredLamp.fs"),
	lightingShader(Filepath::DeferredShader + "DeferredShading.vs", Filepath::DeferredShader + "DeferredShading.fs"),
	geometryShader(Filepath::DeferredShader + "GBuffer.vs", Filepath::DeferredShader + "GBuffer.fs")
{
}

DeferredShading::~DeferredShading()
{
}

void DeferredShading::Initialize(Scene & scene)
{
	gBuffer.Generate();
	gBuffer.Bind();
	
	Window::Parameters parameters = window.GetWindowParameters();
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

	lightingShader.Use();
	lightingShader.SetInt("gPosition", 0);
	lightingShader.SetInt("gNormal", 1);
	lightingShader.SetInt("gAlbedoSpecular", 2);

	geometryShader.Use();
	geometryShader.SetInt("diffuse", 0);
	geometryShader.SetInt("specular", 1);

	glViewport(0, 0, parameters.Width, parameters.Height);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void DeferredShading::Render(Scene & scene)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Geometry pass
	gBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = scene.GetCamera().GetProjectionMatrix();
	glm::mat4 view = scene.GetCamera().GetViewMatrix();

	geometryShader.Use();
	geometryShader.SetMat4("projection", projection);
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

	//Lighting pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	lightingShader.Use();
	for (unsigned int i = 0; i < 3; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBufferTextures[i].GetID());
	}
	//Send light relevant uniforms
	const std::vector<Light>& lights = scene.GetLights();
	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		lightingShader.SetVec3("lights[" + std::to_string(i) + "].Position", lights[i].GetWorldPosition());
		lightingShader.SetVec3("lights[" + std::to_string(i) + "].Color", lights[i].GetColour());
		//Update attenuation parameters and calculate radius
		const float linear = 0.35f;
		const float quadratic = 0.44f;
		lightingShader.SetFloat("lights[" + std::to_string(i) + "].Linear", linear);
		lightingShader.SetFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
	}
	lightingShader.SetVec3("viewPosition", scene.GetCamera().GetWorldPosition());
	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);

	//Copy content of geometry shader to default framebuffer's depth buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.GetBuffer());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format)
	Window::Parameters parameter = window.GetWindowParameters();

	glBlitFramebuffer(0, 0, parameter.Width, parameter.Height, 0, 0, parameter.Width, parameter.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	gBuffer.Unbind();

	//Render lights on top of scene
	lamp.Use();
	lamp.SetMat4("projection", projection);
	lamp.SetMat4("view", view);

	for (unsigned int i = 0; i < lights.size(); ++i)
	{
		lamp.SetMat4("model", lights[i].GetWorldMatrix());
		lamp.SetVec3("lightColour", lights[i].GetColour());
		lights[i].GetRenderComponent().GetMesh().Draw();
	}
}
