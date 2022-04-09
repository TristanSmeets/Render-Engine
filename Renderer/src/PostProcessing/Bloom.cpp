#include "Rendererpch.h"
#include "Bloom.h"
#include "gtc/constants.hpp"

Bloom::Bloom()
{
}

Bloom::~Bloom()
{
}

void Bloom::Initialize(const Window::Parameters & parameters)
{
	bloom.CompileShader(Filepath::ForwardShader + "BasicPostProcessing.vs");
	bloom.CompileShader(Filepath::ForwardShader + "Bloom.fs");
	bloom.Link();
	bloom.Validate();
	
	SetupHDRFramebuffer(parameters);
	SetupShaders();
	gaussian.SetupFramebuffers(glm::ivec2(parameters.Width, parameters.Height));

	bloomTexture = Texture::CreateEmpty("Bloom", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Bloom::SetupShaders()
{
	bloom.Use();
	bloom.SetUniform("scene", 0);
	bloom.SetUniform("bloomBlur", 1);
}

void Bloom::SetupHDRFramebuffer(const Window::Parameters & parameters)
{
	hdrFBO.Generate();
	hdrFBO.Bind();

	for (unsigned int i = 0; i < 2; ++i)
	{
		colourBuffers[i] = Texture::CreateEmpty("ColourBuffer#" + std::to_string(i), parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		hdrFBO.AttachTexture(GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colourBuffers[i].GetID());
	}

	depthRenderbuffer.Generate();
	depthRenderbuffer.Bind();
	depthRenderbuffer.SetStorage(GL_DEPTH_COMPONENT, parameters.Width, parameters.Height);
	hdrFBO.AttachRenderbuffer(GL_DEPTH_ATTACHMENT, depthRenderbuffer);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	if (!hdrFBO.IsCompleted())
	{
		printf("ERROR: hdrFBO isn't complete!\n");
	}

	hdrFBO.Unbind();
}

void Bloom::Bind()
{
	hdrFBO.Bind();
}

void Bloom::Unbind()
{
	hdrFBO.Unbind();
}

void Bloom::BlurTextureBuffers()
{
	gaussian.BlurTexture(colourBuffers[1], bloomTexture, parameters.BlurParameters);
}

void Bloom::Draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

void Bloom::Apply()
{
	colourBuffers[0].Bind(bloom, Texture::Type::Albedo);
	bloomTexture.Bind(bloom, Texture::Type::Normal);
}

void Bloom::Apply(const Bloom::Parameters& parameters)
{
	BlurTextureBuffers();
	bloom.Use();
	bloom.SetUniform("exposure", parameters.Exposure);
	bloom.SetUniform("gammaCorrection", parameters.GammaCorrection);
	Apply();
}

const Framebuffer & Bloom::GetFramebuffer() const
{
	return hdrFBO;
}

const Texture & Bloom::GetTexture() const
{
	return colourBuffers[0];
}

