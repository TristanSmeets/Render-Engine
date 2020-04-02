#pragma once

#include "Rendering/Texture.h"
#include "Rendering/Shader.h"

class Material
{
public:
	Material(const std::string& name);
	~Material();
	void AddTexture(Texture::Type type, const std::string& filepath, bool usingLinearSpace = false);
	void AddTexture(Texture::Type type, const Texture& texture);
	Texture& GetTexture(Texture::Type type);

private:
	Texture textures[Texture::Type::Count];
	std::string name;
};