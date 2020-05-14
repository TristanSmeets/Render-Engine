#include "Rendererpch.h"
#include "MSAA.h"

MSAA::MSAA()
{
}

MSAA::~MSAA()
{
}

void MSAA::Initialize(const Parameters & parameters)
{
	multiSampleBuffer.Generate();
	multiSampleBuffer.Bind();

	Texture::MultiSampleParameters textureParameters;
	textureParameters.Resolution = parameters.Resolution;
	textureParameters.Samples = parameters.Samples;
	textureParameters.InternalFormat = parameters.TextureFormat;
	textureParameters.FixedSampleLocations = GL_TRUE;
	textureParameters.Name = "MultiSample";
	multiSampleColourAttachment = Texture::CreateMultiSample(textureParameters);
	multiSampleBuffer.AttachTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multiSampleColourAttachment.GetID());

	renderbuffer.Generate();
	renderbuffer.Bind();
	Renderbuffer::MultiSampleParameters renderbufferParameters;
	renderbufferParameters.Samples = parameters.Samples;
	renderbufferParameters.InternalFormat = GL_DEPTH24_STENCIL8;
	renderbufferParameters.Resolution = parameters.Resolution;
	renderbuffer.SetStorageMultiSample(renderbufferParameters);
	renderbuffer.Unbind();
	multiSampleBuffer.AttachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, renderbuffer);

	if (!multiSampleBuffer.IsCompleted())
	{
		printf("ERROR: Multisample framebuffer not complete!");
	}
	multiSampleBuffer.Unbind();
}

void MSAA::Bind()
{
	multiSampleBuffer.Bind();
}

void MSAA::Blit(const Framebuffer::BlitParameters &parameters)
{
	multiSampleBuffer.BlitFramebuffer(parameters);
}

const Framebuffer & MSAA::GetMultiSampleBuffer() const
{
	return multiSampleBuffer;
}
