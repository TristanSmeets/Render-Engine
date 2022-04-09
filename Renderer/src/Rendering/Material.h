#pragma once

#include "Rendering/Texture.h"
#include "Rendering/GLSLProgram.h"

class Material
{
public:
	Material(const std::string& name);
	~Material();
	void AddTexture(Texture::Type type, const std::string& filepath, bool usingLinearSpace = false);
	void AddTexture(Texture::Type type, const Texture& texture);
	void BindTextures() const;
	const Texture& GetTexture(Texture::Type type) const;
	const std::string& GetName() const;

private:
	Texture textures[(int)Texture::Type::Count];
	std::string name;
};