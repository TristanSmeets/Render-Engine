#include "Rendererpch.h"
#include "DepthOfField.h"

DepthOfField::DepthOfField() : 
	depthOfField(Shader(Filepath::ForwardShader + "BasicPostProcessing.vs", Filepath::PostProcessing + "DepthOfField.fs"))
{
}

DepthOfField::~DepthOfField()
{
}

void DepthOfField::Initialize(const Window::Parameters & parameters)
{
	SetupFramebuffer(parameters);
	SetupShaders();
	gaussian.SetupFramebuffers(glm::ivec2(parameters.Width, parameters.Height));

	blurredScene = Texture::CreateEmpty("BlurredScene", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void DepthOfField::Bind()
{
	framebuffer.Bind();
}

void DepthOfField::Unbind()
{
	framebuffer.Unbind();
}

void DepthOfField::Draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

void DepthOfField::Apply()
{
	depthOfField.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colourBuffer.GetID());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, blurredScene.GetID());
}

void DepthOfField::Apply(const DepthOfField::Parameters& parameters)
{
	gaussian.BlurTexture(colourBuffer, blurredScene, parameters.Lod, 1);
	depthOfField.Use();
	depthOfField.SetFloat("focalDistance", parameters.FocalDistance);
	depthOfField.SetFloat("focalRange", parameters.FocalRange);
	depthOfField.SetFloat("rangeCutoff", parameters.RangeCutoff);
	Apply();
}

void DepthOfField::Draw(const Texture & depthTexture)
{
	depthOfField.Use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTexture.GetID());
	Draw();
}

const Framebuffer & DepthOfField::GetFramebuffer() const
{
	return framebuffer;
}

const Texture & DepthOfField::GetTexture() const
{
	return colourBuffer;
}

void DepthOfField::SetupFramebuffer(const Window::Parameters& parameters)
{
	framebuffer.Generate();
	framebuffer.Bind();

	colourBuffer = Texture::CreateEmpty("ColourBuffer", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	framebuffer.AttachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer.GetID());
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	renderbuffer.Generate();
	renderbuffer.Bind();
	renderbuffer.SetStorage(GL_DEPTH_COMPONENT, parameters.Width, parameters.Height);
	framebuffer.AttachRenderbuffer(GL_DEPTH_ATTACHMENT, renderbuffer);

	if (!framebuffer.IsCompleted())
	{
		printf("ERROR: Depth of field framebuffer not complete\n");
	}
	framebuffer.Unbind();
}

void DepthOfField::SetupShaders()
{
	depthOfField.Use();
	depthOfField.SetInt("scene", 0);
	depthOfField.SetInt("depthTexture", 1);
	depthOfField.SetInt("blurredScene", 2);
}
