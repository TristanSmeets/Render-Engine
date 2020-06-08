#include "Rendererpch.h"
#include "Bloom.h"
#include "gtc/constants.hpp"

Bloom::Bloom() :
	//blur(Shader(Filepath::ForwardShader + "BasicPostProcessing.vs", Filepath::ForwardShader + "Blur.fs")),
	bloom(Shader(Filepath::ForwardShader + "BasicPostProcessing.vs", Filepath::ForwardShader + "Bloom.fs"))
{
}

Bloom::~Bloom()
{
}

void Bloom::Initialize(const Window::Parameters & parameters)
{
	SetupHDRFramebuffer(parameters);
	//SetupBlurFramebuffer(parameters);
	SetupShaders();
	gaussian.SetupFramebuffers(glm::ivec2(parameters.Width, parameters.Height));

	bloomTexture = Texture::CreateEmpty("Bloom", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	blurredScene = Texture::CreateEmpty("BlurredScene", parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void Bloom::SetupShaders()
{
	//blur.Use();
	//blur.SetInt("image", 0);
	bloom.Use();
	bloom.SetInt("scene", 0);
	bloom.SetInt("bloomBlur", 1);
	bloom.SetInt("depthTexture", 2);
	bloom.SetInt("blurredScene", 3);
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
	gaussian.BlurTexture(colourBuffers[1], bloomTexture, parameters.MaxLOD, parameters.BlurLoops);
	gaussian.BlurTexture(colourBuffers[0], blurredScene, parameters.MaxLOD, parameters.BlurLoops);
}

void Bloom::Draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	bloom.Use();
	bloom.SetFloat("focalDistance", parameters.FocalDistance);
	bloom.SetFloat("focalRange", parameters.FocalRange);
	colourBuffers[0].Bind(bloom, Texture::Albedo);
	bloomTexture.Bind(bloom, Texture::Normal);
	bloom.SetFloat("exposure", parameters.Exposure);
	bloom.SetFloat("gammaCorrection", parameters.GammaCorrection);
	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

void Bloom::Apply()
{
	BlurTextureBuffers();
}

void Bloom::Draw(const Texture & depth)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	bloom.Use();
	bloom.SetFloat("focalDistance", parameters.FocalDistance);
	bloom.SetFloat("focalRange", parameters.FocalRange);
	bloom.SetFloat("rangeCutoff", parameters.RangeCutoff);
	bloom.SetFloat("exposure", parameters.Exposure);
	bloom.SetFloat("gammaCorrection", parameters.GammaCorrection);

	colourBuffers[0].Bind(bloom, Texture::Albedo);
	bloomTexture.Bind(bloom, Texture::Normal);
	depth.Bind(bloom, Texture::Metallic);
	blurredScene.Bind(bloom, Texture::Roughness);
	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

const Framebuffer & Bloom::GetFramebuffer() const
{
	return hdrFBO;
}

const Texture & Bloom::GetTexture() const
{
	return colourBuffers[0];
}

