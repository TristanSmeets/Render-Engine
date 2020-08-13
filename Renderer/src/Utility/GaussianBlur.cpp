#include "Rendererpch.h"
#include "GaussianBlur.h"
#include "Utility/Filepath.h"
#include <gtc/matrix_transform.hpp>

GaussianBlur::GaussianBlur() :
	blur(Shader(Filepath::ForwardShader + "BasicPostProcessing.vs", Filepath::ForwardShader + "Blur.fs"))
{
	SetupShader();
}

GaussianBlur::~GaussianBlur()
{
	glDeleteTextures(1, &textures[0].GetID());
	glDeleteTextures(1, &textures[1].GetID());
}

void GaussianBlur::BlurTexture(const Texture & source, Texture & destination, unsigned int maxLOD, int blurLoops)
{
	for (int i = 0; i < 2; ++i)
	{
		framebuffers[i].Bind();
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearTexImage(textures[i].GetID(), 0, GL_RGB16F, GL_RGB, 0);
	}

	bool horizontal = true;
	bool firstIteration = true;

	glm::ivec2 copyResolution = source.GetResolution();

	glCopyImageSubData(
		source.GetID(), GL_TEXTURE_2D, 0, 0, 0, 0,
		textures[0].GetID(), GL_TEXTURE_2D, 0, 0, 0, 0,
		copyResolution.x, copyResolution.y, 1);

	blur.Use();
	blur.SetInt("MaxLod", maxLOD);

	for (int i = 0; i < blurLoops * 2; ++i)
	{
		if (horizontal)
		{
			blur.SetSubroutine(Shader::SubroutineParameters("Horizontal", GL_FRAGMENT_SHADER));
		}
		else
		{
			blur.SetSubroutine(Shader::SubroutineParameters("Vertical", GL_FRAGMENT_SHADER));
		}
		framebuffers[horizontal].Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[!horizontal].GetID());
		quad.Render();
		glGenerateMipmap(GL_TEXTURE_2D);

		horizontal = !horizontal;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	framebuffers[0].Unbind();

	glCopyImageSubData(
		textures[(blurLoops * 2) % 2].GetID(), GL_TEXTURE_2D, 0, 0, 0, 0,
		destination.GetID(), GL_TEXTURE_2D, 0, 0, 0, 0,
		copyResolution.x, copyResolution.y, 1);
}

void GaussianBlur::SetupShader()
{
	blur.Use();

	float weights[5];
	float sum;
	float sigma2 = 4.0f;
	//Compute and sum blur weights
	weights[0] = Gauss(0, sigma2);
	sum = weights[0];
	for (int i = 1; i < 5; i++)
	{
		weights[i] = Gauss(i, sigma2);
		sum += 2 * weights[i];
	}

	//Normalize the weights and set the uniform
	for (int i = 0; i < 5; i++)
	{
		blur.SetFloat("weight[" + std::to_string(i) + "]", weights[i] / sum);
	}

	blur.SetInt("image", 0);
}

float GaussianBlur::Gauss(float x, float sigma2)
{
	double coefficient = 1.0 / (glm::two_pi<double>() * sigma2);
	double exponant = -(x * x) / (2.0 * sigma2);
	return (float)(coefficient * exp(exponant));
}

void GaussianBlur::SetupFramebuffers(const glm::ivec2& resolution)
{
	for (unsigned int i = 0; i < 2; ++i)
	{
		framebuffers[i].Generate();
		framebuffers[i].Bind();
		textures[i] = Texture::CreateEmpty("Blur#" + std::to_string(i), resolution.x, resolution.y, GL_RGB16F, GL_RGB, GL_FLOAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 7);
		glGenerateMipmap(GL_TEXTURE_2D);
		framebuffers[i].AttachTexture(textures[i]);
		if (!framebuffers->IsCompleted())
		{
			printf("Error: blur framebuffer%d not complete", 1);
		}
	}
}
