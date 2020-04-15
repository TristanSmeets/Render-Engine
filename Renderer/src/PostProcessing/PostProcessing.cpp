#include "Rendererpch.h"
#include "PostProcessing.h"
#include "Utility/Filepath.h"

PostProcessing::PostProcessing() : 
	shader(Shader(Filepath::Shader + "BasicPostProcessing.vs", Filepath::Shader + "BasicPostProcessing.fs"))
{
}

PostProcessing::~PostProcessing()
{
	glDeleteTextures(1, &empty.GetID());
}

void PostProcessing::Initialize(const Window::Parameters & parameters)
{
	framebuffer.Generate();
	framebuffer.Bind();

	empty = Texture::CreateEmpty("ColourBuffer", parameters.Width, parameters.Height, GL_RGBA16, GL_RGB, GL_UNSIGNED_INT);
	framebuffer.AttachTexture(empty);

	renderbuffer.Generate();
	renderbuffer.Bind();
	renderbuffer.SetStorage(GL_DEPTH24_STENCIL8, parameters.Width, parameters.Height);
	framebuffer.AttachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, renderbuffer);

	if (!framebuffer.IsCompleted())
	{
		printf("Error: Framebuffer not complete.\n");
	}
	framebuffer.Unbind();

	shader.Use();
	shader.SetInt("colourTexture", 0);
}

void PostProcessing::Bind()
{
	framebuffer.Bind();
}

void PostProcessing::Unbind()
{
	framebuffer.Unbind();
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessing::Draw()
{
	shader.Use();
	shader.SetFloat("gammaCorrection", 2.2f);
	shader.SetFloat("exposure", 1.0f);
	empty.Bind(shader, Texture::Albedo);
	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}
