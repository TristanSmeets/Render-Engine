#include "Rendererpch.h"
#include "Basic.h"

Basic::Basic() : PostProcessing()
{
}

Basic::~Basic()
{
	glDeleteTextures(1, &empty.GetID());
}

void Basic::Initialize(const Window::Parameters & parameters)
{
	shader.CompileShader(Filepath::ForwardShader + "BasicPostProcessing.vs");
	shader.CompileShader(Filepath::ForwardShader + "BasicPostProcessing.fs");
	shader.Link();
	shader.Validate();
	
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
	shader.SetUniform("colourTexture", 0);
}

void Basic::Bind()
{
	framebuffer.Bind();
}

void Basic::Unbind()
{
	framebuffer.Unbind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Basic::Draw()
{
	shader.Use();
	shader.SetUniform("gammaCorrection", parameters.GammaCorrection);
	shader.SetUniform("exposure", parameters.Exposure);
	empty.Bind(shader, Texture::Albedo);
	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

void Basic::Apply()
{
}

const Framebuffer & Basic::GetFramebuffer() const
{
	return framebuffer;
}

const Texture & Basic::GetTexture() const
{
	return empty;
}
