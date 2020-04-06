#include "Rendererpch.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() :
	name("Empty")
{
}

Texture::Texture(const std::string & name, GLuint id) :
	name(name), id(id)
{
}

Texture::Texture(const std::string & filepath, bool usingLinearSpace)
{
	GenerateTexture();
	Load(filepath, usingLinearSpace);

	size_t position = filepath.find_last_of("/");
	name = filepath.substr(position + 1);
}

Texture::Texture(const std::string & filepath, GLenum internalformat, GLenum format, GLenum type)
{
	printf("Creating Texture: %s\n", filepath.c_str());
	GenerateTexture();
	Load(filepath, internalformat, format, type);

	size_t position = filepath.find_last_of("/");
	name = filepath.substr(position + 1);
}

Texture::Texture(const Texture & rhs) :
	id(rhs.id), name("[COPY]" + rhs.name)
{
}

Texture::~Texture()
{
	//TODO: Add glDeleteTextures. Need to figure out how to delete a texture without it breaking for all the copies as well.
}

Texture Texture::CreateEmpty(const std::string& name, int width, int height, GLenum internalformat, GLenum format, GLenum type)
{
	Texture emptyTexture;
	emptyTexture.name = name;
	emptyTexture.GenerateTexture();
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, 0);
	return emptyTexture;
}

const GLuint & Texture::GetID() const
{
	return id;
}

const std::string & Texture::GetName() const
{
	return name;
}

void Texture::Bind(Shader & shader, Type type) const
{
	glActiveTexture(GL_TEXTURE0 + type);
	shader.SetInt(EnumToString(type), type);

	glBindTexture(GL_TEXTURE_2D, GetID());

	//Return back to default texture
	glActiveTexture(GL_TEXTURE0);
}

void Texture::GenerateTexture()
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	//Set texture wrapping/filtering options
	//Wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::Load(const std::string & filepath, bool usingLinearSpace)
{
	int width;
	int height;
	int nrChannels;

	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		GLenum format;
		GLenum gammaCorrection;

		switch (nrChannels)
		{
		case 1:
			format = GL_RED;
			gammaCorrection = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			gammaCorrection = GL_SRGB;
			break;
		case 4:
			format = GL_RGBA;
			gammaCorrection = GL_SRGB_ALPHA;
			break;
		}
		glBindTexture(GL_TEXTURE_2D, id);

		if (usingLinearSpace)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, gammaCorrection, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
		printf("SUCCESS: Loaded: %s\n", filepath.c_str());
	}
	else
	{
		printf("ERROR: Failed to load texture: %s\n", filepath.c_str());
	}

	stbi_set_flip_vertically_on_load(false);
}

void Texture::Load(const std::string & filepath, GLenum internalformat, GLenum format, GLenum type)
{
	int width;
	int height;
	int nrChannels;

	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else
	{
		printf("ERROR: Failed to load texture: %s\n", filepath.c_str());
	}

	stbi_set_flip_vertically_on_load(false);
}

const std::string Texture::EnumToString(Type type) const
{
	switch (type)
	{
	case Albedo:
		return "material.Albedo";
	case Normal:
		return "material.Normal";
	case Metallic:
		return "material.Metallic";
	case Roughness:
		return "material.Roughness";
	case AmbientOcclusion:
		return "material.AO";
	//case LookUp:
	//	return "brdfLUT";
	default:
		return "";
	}
}
