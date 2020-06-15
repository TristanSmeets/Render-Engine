#include "Rendererpch.h"
#include "SSAO.h"

SSAO::SSAO() : 
	ssao(Shader(Shader(Filepath::DeferredShader + "PBR/PBRLighting.vs", Filepath::DeferredShader + "ADS/SSAO.fs"))),
	blur(Shader(Filepath::DeferredShader + "PBR/PBRLighting.vs", Filepath::DeferredShader + "ADS/SSAOBlur.fs"))
{
}

SSAO::~SSAO()
{
}

void SSAO::Initialize(const Window::Parameters & parameters)
{
	//Setting up framebuffers
	for (int i = 0; i < 2; ++i)
	{
		buffers[i].Generate();
		buffers[i].Bind();
		textures[i] = Texture::CreateEmpty("AO_Texture#" + std::to_string(i), parameters.Width, parameters.Height, GL_RED, GL_RGB, GL_FLOAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		buffers[i].AttachTexture(textures[i]);
		if (!buffers[i].IsCompleted())
		{
			printf("AO buffer#%d is not complete\n", i);
		}
		buffers[i].Unbind();
	}

	std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;
	CreateKernel(randomFloats, generator);
	CreateNoiseTexture(randomFloats, generator);
}

void SSAO::Initialize(const Window::Parameters & parameters, const glm::mat4 & projection)
{
	Initialize(parameters);
	SetupShaders(projection);
}

void SSAO::Bind()
{
	buffers[0].Bind();
}

void SSAO::Unbind()
{
	buffers[0].Unbind();
}

void SSAO::Draw()
{
}

void SSAO::Apply()
{
	TextureBlurPass();
}

void SSAO::Apply(const Texture & positions, const Texture & normals, const SSAO::Parameters& parameters)
{
	this->parameters = parameters;
	TexturCreationPass(positions, normals);
	Apply();
}

const Framebuffer & SSAO::GetFramebuffer() const
{
	return buffers[0];
}

const Texture & SSAO::GetTexture() const
{
	return textures[1];
}

void SSAO::CreateNoiseTexture(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine & generator)
{
	glm::vec3 ssaoNoise[16];
	for (unsigned int i = 0; i < 16; ++i)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f);
		ssaoNoise[i] = noise;
	}
	GLuint noiseTexture;
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	noise = Texture("aoNoise", noiseTexture);
}

void SSAO::CreateKernel(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine & generator)
{
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		//scale samples so they're more aligned to the center of the kernel
		scale = 0.1f + (scale * scale) * 0.9f;
		sample *= scale;
		kernel[i] = sample;
	}
}

void SSAO::TexturCreationPass(const Texture& positions, const Texture& normals)
{
	buffers[0].Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	ssao.Use();
	ssao.SetInt("occlusionPower", parameters.OcclusionPower);
	ssao.SetInt("kernelSize", parameters.KernelSize);
	ssao.SetFloat("radius", parameters.Radius);
	ssao.SetFloat("bias", parameters.Bias);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, positions.GetID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normals.GetID());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noise.GetID());
	quad.Render();
	buffers[0].Unbind();
}

void SSAO::TextureBlurPass()
{
	buffers[1].Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	blur.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0].GetID());
	quad.Render();
	buffers[1].Unbind();
}

void SSAO::SetupShaders(const glm::mat4& projection)
{
	ssao.Use();
	ssao.SetInt("gPosition", 0);
	ssao.SetInt("gNormal", 1);
	ssao.SetInt("noise", 2);
	ssao.SetMat4("projection", projection);
	for (unsigned int i = 0; i < 64; ++i)
	{
		ssao.SetVec3("samples[" + std::to_string(i) + "]", kernel[i]);
	}

	blur.Use();
	blur.SetInt("ssaoInput", 0);

}
