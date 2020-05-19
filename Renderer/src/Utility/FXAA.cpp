#include "Rendererpch.h"
#include "FXAA.h"
#include "Utility/Filepath.h"

FXAA::FXAA() : 
	shader(Shader(Filepath::DeferredShader + "PBR/PBRLighting.vs", Filepath::DeferredShader + "FXAA.fs"))
{
}

FXAA::~FXAA()
{
}

void FXAA::Initialize(const Parameters & parameters)
{
	fxaaParameters = parameters;
	framebuffer.Generate();
	framebuffer.Bind();

	fxaaColourAttachment = Texture::CreateEmpty("FxaaColour", parameters.Resolution.x, parameters.Resolution.y, GL_RGB16F, GL_RGB, GL_UNSIGNED_INT);
	framebuffer.AttachTexture(fxaaColourAttachment);

	renderbuffer.Generate();
	renderbuffer.Bind();
	renderbuffer.SetStorage(GL_DEPTH24_STENCIL8, parameters.Resolution.x, parameters.Resolution.y);
	renderbuffer.Unbind();
	
	framebuffer.AttachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, renderbuffer);

	if (!framebuffer.IsCompleted())
	{
		printf("Error: FXAA framebuffer not complete\n");
	}
	shader.Use();
	shader.SetInt("colourTexture", 0);
}

void FXAA::Bind()
{
	framebuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FXAA::Unbind()
{
	framebuffer.Unbind();
}

void FXAA::Blit(const Framebuffer::BlitParameters & parameters)
{
	framebuffer.BlitFramebuffer(parameters);
}

void FXAA::Apply(const Parameters& parameters, const Texture& screenTexture)
{
	shader.Use();
	shader.SetFloat("spanMax", parameters.SpanMax);
	shader.SetFloat("reduceMinimum", parameters.ReduceMinumum);
	shader.SetFloat("reduceMultiplier", parameters.ReduceMultiplier);
	shader.SetVec3("inverseFilterTextureSize", glm::vec3((1.0f / fxaaParameters.Resolution.x), (1.0f / fxaaParameters.Resolution.y), 0.0f));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fxaaColourAttachment.GetID());
	glDisable(GL_DEPTH_TEST);
	quad.Render();
	glEnable(GL_DEPTH_TEST);
}

const Framebuffer & FXAA::GetFramebuffer() const
{
	return framebuffer;
}
