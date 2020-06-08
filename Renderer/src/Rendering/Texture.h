#pragma once

#include "glad/glad.h"
#include "Rendering/Shader.h"

class Texture
{
public:
	enum Type
	{
		Albedo,
		Normal,
		Metallic,
		Roughness,
		AmbientOcclusion,
		Count
	};

	struct MultiSampleParameters
	{
		std::string Name;
		GLsizei Samples;
		GLint InternalFormat;
		glm::ivec2 Resolution;
		GLboolean FixedSampleLocations;
	};
public:
	Texture();
	Texture(const std::string& name, GLuint id);
	Texture(const std::string& filepath, bool usingLinearSpace = false);
	Texture(const std::string& filepath, GLenum internalformat, GLenum format, GLenum type);
	Texture(const Texture& rhs);
	~Texture();
	static Texture CreateEmpty(const std::string& name, int width, int height, GLenum internalformat, GLenum format, GLenum type);
	static Texture CreateMultiSample(const MultiSampleParameters& parameters);
	static const std::string TypeToString(Type type);
	const GLuint& GetID() const;
	const std::string& GetName() const;
	void Bind(Shader& shader, Type type) const;
	Texture& operator=(const Texture& rhs);
	const glm::ivec2& GetResolution() const;
private:
	void GenerateTexture();
	void Load(const std::string& filepath, bool usingLinearSpace);
	void Load(const std::string& filepath, GLenum internalformat, GLenum format, GLenum type);
	const std::string EnumToString(Type type) const;
	
	std::string name = "Empty";
	GLuint id = 0;
	glm::ivec2 resolution = glm::ivec2(1280, 720);
};