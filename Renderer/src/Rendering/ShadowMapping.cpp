#include "Rendererpch.h"
#include "ShadowMapping.h"
#include "Utility/Filepath.h"
#include <gtc/matrix_transform.hpp>

ShadowMapping::ShadowMapping() :
	shadowDepth(Shader(Filepath::ForwardShader + "PointLightDepthMap.vs", Filepath::ForwardShader + "PointLightDepthMap.fs", Filepath::ForwardShader + "PointLightDepthMap.gs"))
{
}

ShadowMapping::~ShadowMapping()
{
	for (int i = 0; i < MaximumNumberOfLights; ++i)
	{
		glDeleteTextures(1, &cubeMaps[i].GetID());
	}
}

void ShadowMapping::Initialize(const ShadowMapping::Parameters& parameters)
{
	SetParameters(parameters);
	SetupPointLightBuffer();
}

void ShadowMapping::MapPointLights(const std::vector<Light>& lights, const std::vector<Actor>& actors, int lightsToRender)
{
	glViewport(0, 0, parameters.Resolution.x, parameters.Resolution.y);
	depthBuffer.Bind();
	shadowDepth.Use();
	shadowDepth.SetFloat("farPlane", parameters.FarPlane);

	glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), parameters.AspectRatio, parameters.NearPlane, parameters.FarPlane);

	glCullFace(GL_FRONT);
	for (int i = 0; i < lightsToRender; ++i)
	{
		const Cubemap& shadowCubeMap = cubeMaps[i];
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubeMap.GetID(), 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		const Light& light = lights[i];
		const glm::vec3 lightPosition = light.GetWorldPosition();
		shadowDepth.SetVec3("lightPosition", lightPosition);
		
		glm::mat4 shadowTransforms[6] =
		{
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		for (unsigned int i = 0; i < 6; ++i)
		{
			shadowDepth.SetMat4(std::string("shadowMatrices[") + std::to_string(i) + std::string("]"), shadowTransforms[i]);
		}

		for (int i = 0; i < actors.size(); ++i)
		{
			shadowDepth.SetMat4("model", actors[i].GetWorldMatrix());
			actors[i].GetRenderComponent().GetMesh().Draw();
		}
	}

	glCullFace(GL_BACK);
	depthBuffer.Unbind();
}

void ShadowMapping::SetParameters(const Parameters & parameters)
{
	this->parameters.AspectRatio = parameters.AspectRatio;
	this->parameters.FarPlane = parameters.FarPlane;
	this->parameters.NearPlane = parameters.NearPlane;
	this->parameters.Resolution = parameters.Resolution;
}

const ShadowMapping::Parameters & ShadowMapping::GetParameters() const
{
	return parameters;
}

const int ShadowMapping::GetMaximumNumberOfLights() const
{
	return MaximumNumberOfLights;
}

void ShadowMapping::BindShadowMap(int index)
{
	if (index > MaximumNumberOfLights)
	{
		return;
	}
	cubeMaps[index].Bind();
}

void ShadowMapping::SetupPointLightBuffer()
{
	for (int i = 0; i < MaximumNumberOfLights; ++i)
	{
		Cubemap& shadowMap = cubeMaps[i];
		shadowMap.CreateTexture(parameters.Resolution.x, parameters.Resolution.y, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
		shadowMap.SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		shadowMap.SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	depthBuffer.Generate();
	depthBuffer.Unbind();
}
