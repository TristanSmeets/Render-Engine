#pragma once

#include "Utility/Framebuffer.h"
#include "Utility/Renderbuffer.h"
#include "Utility/NDCQuad.h"
#include "Core/Window.h"
#include "Rendering/GLSLProgram.h"
#include "Rendering/Texture.h"
#include "Utility/Filepath.h"

enum ToneMapping
{
	NONE,
	SIMPLE,
	ACES_SIMPLE,
	JOHN_HABLE,
	COUNT
};

class PostProcessing
{
public:
	struct Parameters
	{
		// Tone Mapping
		ToneMapping Type = ACES_SIMPLE;
		float Exposure = 0.6f;
		
		// Gamma Correction
		float Correction = 2.2f;
		bool UseGammaCorrection = true;
		
		// FXAA
		glm::vec2 TexelStep = glm::vec2(1.0f / 1280.0f, 1.0f / 720.0f);
		float LumaThreshold = 1.0f / 8.0f;
		float MaxSpan = 8.0f;
		float ReductionMinimum = 1.0f / 16.0f;
		float ReductionMultiplier = 1.0f / 8.0f;
		bool UseFXAA = true;
		bool ShowEdges = false;
	};
	
public:
	PostProcessing();
	virtual ~PostProcessing();

	/* Compiles all the shaders and setups the frame buffer.
	 * @param const Window::Parameters& parameters
	 */
	virtual void Initialize(const Window::Parameters& parameters);

	/* Binds the post processing frame buffer */
	virtual void Bind();

	/* Unbinds the post processing frame buffer */
	virtual void Unbind();

	/* Applies the post processing effects */
	virtual void Apply();

	/* Sets all the shader uniforms and calls Apply() */
	void SetUniforms(const Parameters& parameters);

	/* Gets the framebuffer used for the post processing
	 * @return Framebuffer& framebuffer
	 */
	virtual const Framebuffer& GetFramebuffer() const;

	/* Gets the texture the post processing framebuffer writes to.
	 * @return Texture& colourAttachment
	 */
	virtual const Texture& GetTexture() const;

private:
	GLSLProgram program;
	Framebuffer framebuffer;
	Renderbuffer renderbuffer;
	Texture colourAttachment;
	NDCQuad quad;
	Parameters parameters;
};
