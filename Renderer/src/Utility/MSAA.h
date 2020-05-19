#pragma once
#include "glm.hpp"
#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"

class MSAA
{
public:
	struct Parameters 
	{
		glm::ivec2 Resolution;
		int Samples;
		GLint TextureFormat;
	};
public:
	MSAA();
	~MSAA();
	void Initialize(const Parameters& parameters);
	void Bind();
	void Blit(const Framebuffer::BlitParameters& parameters);
	const Framebuffer& GetMultiSampleBuffer() const;

private:
	Framebuffer multiSampleBuffer;
	Renderbuffer renderbuffer;
	Texture multiSampleColourAttachment;
};