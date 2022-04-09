#include "Rendererpch.h"
#include "Material.h"

Material::Material(const std::string & name) : 
	name(name)
{
}

Material::~Material()
{
}

void Material::AddTexture(Texture::Type type, const std::string & filepath, bool usingLinearSpace)
{
	if (type == Texture::Type::Count)
	{
		return;
	}
	textures[(int)type] = Texture(filepath, usingLinearSpace);
}

void Material::AddTexture(Texture::Type type, const Texture & texture)
{
	if (type == Texture::Type::Count)
	{
		return;
	}
	textures[(int)type] = texture;
}

void Material::BindTextures() const
{
	for (int i = 0; i < (int)Texture::Type::Count; ++i)
	{
		if(textures[i].GetID() == 0)
		{
			continue;
		}
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].GetID());
	}
	glActiveTexture(GL_TEXTURE0);
}

const Texture & Material::GetTexture(Texture::Type type) const
{
	return textures[(int)type];
}

const std::string & Material::GetName() const
{
	return name;
}
