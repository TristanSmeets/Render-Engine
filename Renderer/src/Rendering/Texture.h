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
		LookUp = 7,
		Count
	};

public:
	Texture();
	Texture(const std::string& name, GLuint id);
	Texture(const std::string& filepath, bool usingLinearSpace = false);
	Texture(const std::string& filepath, GLenum internalformat, GLenum format, GLenum type);
	Texture(const Texture& rhs);
	~Texture();
	static Texture CreateEmpty(const std::string& name, int width, int height, GLenum internalformat, GLenum format, GLenum type);
	const GLuint& GetID() const;
	const std::string& GetName() const;
	void Bind(Shader& shader, Type type) const;

private:
	void GenerateTexture();
	void Load(const std::string& filepath, bool usingLinearSpace);
	void Load(const std::string& filepath, GLenum internalformat, GLenum format, GLenum type);
	const std::string EnumToString(Type type) const;
	
	std::string name;
	GLuint id = 0;
};