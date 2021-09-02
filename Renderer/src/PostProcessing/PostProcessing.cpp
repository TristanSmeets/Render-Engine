#include "Rendererpch.h"
#include "PostProcessing.h"


PostProcessing::PostProcessing()
= default;

PostProcessing::~PostProcessing()
{
}

void PostProcessing::Initialize(const Window::Parameters& parameters)
{
	// Setup GLSLProgram
	program.CompileShader(Filepath::PostProcessing + "PostProcessing.vert.glsl");
	program.CompileShader(Filepath::PostProcessing + "PostProcessing.frag.glsl");
	program.Link();
	program.Validate();
	program.Use();
	program.SetUniform("colourTexture", 0);

	// Setup colour attachment.
	colourAttachment = Texture::CreateEmpty("PPColourAttachment", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_UNSIGNED_INT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	// Setup render buffer.
	renderbuffer.Generate();
	renderbuffer.Bind();
	renderbuffer.SetStorage(GL_DEPTH24_STENCIL8, parameters.Width, parameters.Height);
	renderbuffer.Unbind();
	
	// Setup frame buffer.
	framebuffer.Generate();
	framebuffer.Bind();
	framebuffer.AttachTexture(colourAttachment);
	framebuffer.AttachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, renderbuffer);
	Unbind();
}

void PostProcessing::Bind()
{
	framebuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessing::Unbind()
{
	framebuffer.Unbind();
}

void PostProcessing::Apply()
{
	program.Use();
	program.SetUniform("Exposure", 0.6f);
	program.SetUniform("Gamma", 2.2f);
	program.SetUniform("FXAA_Properties.fxaaOn", true);
	program.SetUniform("FXAA_Properties.showEdges", false);
	program.SetUniform("FXAA_Properties.lumaThreshold", (1.0f / 8.0f));
	program.SetUniform("FXAA_Properties.maxSpan", 8.0f);
	program.SetUniform("FXAA_Properties.reductionMinimum", (1.0f / 16.0f));
	program.SetUniform("FXAA_Properties.reductionMultiplier", (1.0f / 8.0f));
	program.SetUniform("FXAA_Properties.texelStep", glm::vec2(1.0f / 1280.0f, 1.0f / 720.0f));
	program.SetUniform("colourTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colourAttachment.GetID());

	// Render results to a full screen quad.
	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

const Framebuffer& PostProcessing::GetFramebuffer() const
{
	return framebuffer;
}

const Texture& PostProcessing::GetTexture() const
{
	return colourAttachment;
}
