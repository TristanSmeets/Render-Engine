#include "Rendererpch.h"
#include "BloomPostProcessing.h"
#include "Utility/Filepath.h"

BloomPostProcessing::BloomPostProcessing() :
	blur(Shader(Filepath::Shader + "BasicPostProcessing.vs", Filepath::Shader + "Blur.fs")),
	bloom(Shader(Filepath::Shader + "BasicPostProcessing.vs", Filepath::Shader + "Bloom.fs"))
{
}

BloomPostProcessing::~BloomPostProcessing()
{
}

void BloomPostProcessing::Initialize(const Window::Parameters & parameters)
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

	for (unsigned int i = 0; i < 2; ++i)
	{
		pingpongFramebuffers[i].Generate();
		pingpongFramebuffers[i].Bind();
		pingpongTextures[i] = Texture::CreateEmpty("PingpongBuffer#" + std::to_string(i), parameters.Width, parameters.Height, GL_RGB16F, GL_RGB, GL_FLOAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		pingpongFramebuffers[i].AttachTexture(pingpongTextures[i]);
		if (!pingpongFramebuffers[i].IsCompleted())
		{
			printf("Error: Pingpong framebuffer#%d not complete", i);
		}
	}
	blur.Use();
	blur.SetInt("image", 0);
	bloom.Use();
	bloom.SetInt("scene", 0);
	bloom.SetInt("bloomBlur", 1);
}

void BloomPostProcessing::BindHDR()
{
	hdrFBO.Bind();
}

void BloomPostProcessing::Unbind()
{
	hdrFBO.Unbind();
}

void BloomPostProcessing::BlurTextureBuffers()
{
	bool horizontal = true;
	bool firstIteration = true;
	unsigned int amount = 10;
	blur.Use();
	for (unsigned int i = 0; i < amount; ++i)
	{
		pingpongFramebuffers[horizontal].Bind();
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
			pingpongTextures[!horizontal].Bind(blur, Texture::Albedo);
		}
		//glBindTexture(GL_TEXTURE_2D, firstIteration ? colourBuffers[1].GetID() : pingpongTextures[!horizontal].GetID());

		quad.Render();
		horizontal = !horizontal;
	}
	pingpongFramebuffers[0].Unbind();
}

void BloomPostProcessing::Draw()
{
	BlurTextureBuffers();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	bloom.Use();
	//blur.Use();
	//blur.SetSubroutine(Shader::SubroutineParameters("Vertical", GL_FRAGMENT_SHADER));
	colourBuffers[0].Bind(bloom, Texture::Albedo);
	//colourBuffers[1].Bind(bloom, Texture::Normal);
	//pingpongTextures[0].Bind(bloom, Texture::Albedo);
	pingpongTextures[0].Bind(bloom, Texture::Normal);
	bloom.SetFloat("exposure", 1.0f);
	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

