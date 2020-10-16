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
		MRAO,
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

	struct Properties
	{
		std::string Name = "Empty";
		glm::ivec2 Resolution = glm::ivec2(1280, 720);
		GLenum InternalFormat = GL_RED;
		GLenum Format = GL_RED;
		GLenum Type = GL_UNSIGNED_BYTE;
	};

public:
	Texture();
	Texture(GLuint id, const Properties& properties);
	Texture(const std::string& filepath, bool usingLinearSpace = false);
	Texture(const std::string& filepath, GLenum internalformat, GLenum format, GLenum type);
	Texture(const Texture& rhs);
	~Texture();
	Texture& operator=(const Texture& rhs);
	
	static Texture CreateEmpty(const Properties& properties);
	static Texture CreateEmpty(const std::string& name, int width, int height, GLenum internalformat, GLenum format, GLenum type);
	static Texture CreateMultiSample(const MultiSampleParameters& parameters);
	static const std::string TypeToString(Type type);
	const GLuint& GetID() const;
	const std::string& GetName() const;
	void Bind(Shader& shader, Type type) const;
	const glm::ivec2& GetResolution() const;
private:
	void GenerateTexture();
	void Load(const std::string& filepath, bool usingLinearSpace);
	void Load(const std::string& filepath, GLenum internalformat, GLenum format, GLenum type);
	const std::string EnumToString(Type type) const;
	
	Properties properties;
	GLuint id = 0;
};