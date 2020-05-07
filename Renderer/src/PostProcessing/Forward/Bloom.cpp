#include "Rendererpch.h"
#include "Bloom.h"

Bloom::Bloom() :
	blur(Shader(Filepath::ForwardShader + "BasicPostProcessing.vs", Filepath::ForwardShader + "Blur.fs")),
	bloom(Shader(Filepath::ForwardShader + "BasicPostProcessing.vs", Filepath::ForwardShader + "Bloom.fs"))
{
}

Bloom::~Bloom()
{
}

void Bloom::Initialize(const Window::Parameters & parameters)
{
	SetupHDRFramebuffer(parameters);
	SetupBlurFramebuffer(parameters);
	SetupShaders();
}

void Bloom::SetupShaders()
{
	blur.Use();
	blur.SetInt("image", 0);
	bloom.Use();
	bloom.SetInt("scene", 0);
	bloom.SetInt("bloomBlur", 1);
}

void Bloom::SetupBlurFramebuffer(const Window::Parameters & parameters)
{

	for (unsigned int i = 0; i < 2; ++i)
	{
		blurFramebuffers[i].Generate();
		blurFramebuffers[i].Bind();
		blurTextures[i] = Texture::CreateEmpty("BlurBuffer#" + std::to_string(i), parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		blurFramebuffers[i].AttachTexture(blurTextures[i]);
		if (!blurFramebuffers[i].IsCompleted())
		{
			printf("Error: blur framebuffer#%d not complete", i);
		}
	}
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

	unsigned int attachements[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachements);

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
	bool horizontal = true;
	bool firstIteration = true;
	unsigned int amount = 10;
	blur.Use();
	for (unsigned int i = 0; i < amount; ++i)
	{
		blurFramebuffers[horizontal].Bind();
		if (horizontal)
		{
			blur.SetSubroutine(Shader::SubroutineParameters("Horizontal", GL_FRAGMENT_SHADER));
		}
		else
		{
			blur.SetSubroutine(Shader::SubroutineParameters("Vertical", GL_FRAGMENT_SHADER));
		}
		if (firstIteration)
		{
			colourBuffers[1].Bind(blur, Texture::Albedo);
			firstIteration = false;
		}
		else
		{
			blurTextures[!horizontal].Bind(blur, Texture::Albedo);
		}

		quad.Render();
		horizontal = !horizontal;
	}
	blurFramebuffers[0].Unbind();
}

void Bloom::Draw()
{
	BlurTextureBuffers();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	bloom.Use();
	colourBuffers[0].Bind(bloom, Texture::Albedo);
	blurTextures[1].Bind(bloom, Texture::Normal);
	bloom.SetFloat("exposure", parameters.Exposure);
	bloom.SetFloat("gammaCorrection", parameters.GammaCorrection);
	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

const Framebuffer & Bloom::GetFramebuffer() const
{
	return hdrFBO;
}

