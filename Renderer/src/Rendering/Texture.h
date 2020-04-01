#pragma once

#include "glad/glad.h"

class Texture
{
	Texture();
	Texture(const std::string& name, GLuint id);
	Texture(const std::string& filepath, bool usingLinearSpace = false);
	Texture(const Texture& rhs);
	~Texture();
	static Texture CreateEmpty(const std::string& name, int width, int height, GLenum format);
	const GLuint& GetID() const;
	const std::string& GetName() const;

private:
	void GenerateTexture();
	void Load(const std::string& filepath, bool usingLinearSpace);
	
	std::string name;
	GLuint id = 0;
};