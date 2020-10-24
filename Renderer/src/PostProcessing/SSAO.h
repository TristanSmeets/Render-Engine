#pragma once
#include "PostProcessing/PostProcessing.h"

class SSAO : public PostProcessing
{
public:
	struct Parameters
	{
		int OcclusionPower = 2;
		int KernelSize = 64;
		float Radius = 0.5f;
		float Bias = 0.025f;
	};
public:
	SSAO();
	virtual ~SSAO();
	void Initialize(const Window::Parameters& parameters);
	void Initialize(const Window::Parameters& parameters, const glm::mat4& projection);
	void Bind();
	void Unbind();
	void Draw();
	void Apply();
	void Apply(const Texture& positions, const Texture& normals, const Parameters& parameters);
	const Framebuffer& GetFramebuffer() const;
	const Texture& GetTexture() const;

private:
	void CreateNoiseTexture(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine& generator);
	void CreateKernel(std::uniform_real_distribution<GLfloat>& randomFloats, std::default_random_engine& generator);
	void TextureCreationPass(const Texture& positions, const Texture& normals);
	void TextureBlurPass();
	void SetupShaders(const glm::mat4& projection);

	Framebuffer buffers[2];
	Texture textures[2];
	Texture noise;
	Shader ssao;
	Shader blur;
	NDCQuad quad;
	Parameters parameters;

	glm::vec3 kernel[64];
};