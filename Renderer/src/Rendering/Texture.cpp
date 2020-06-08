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
	Load(filepath, internalformat, format, type);

	size_t position = filepath.find_last_of("/");
	name = filepath.substr(position + 1);
}

Texture::Texture(const Texture & rhs) :
	id(rhs.id), name(rhs.name)
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
	glBindTexture(GL_TEXTURE_2D, emptyTexture.id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, 0);
	emptyTexture.resolution = glm::ivec2(width, height);
	return emptyTexture;
}

Texture Texture::CreateMultiSample(const MultiSampleParameters & parameters)
{
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, parameters.Samples, parameters.InternalFormat, parameters.Resolution.x, parameters.Resolution.y, parameters.FixedSampleLocations);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	return Texture(parameters.Name, id);
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

Texture & Texture::operator=(const Texture & rhs)
{
	this->id = rhs.id;
	this->name = rhs.name;
	return *this;
}

const glm::ivec2 & Texture::GetResolution() const
{
	return resolution;
}

const std::string Texture::TypeToString(Type type)
{
	switch (type)
	{
	case Texture::Albedo:
		return "Albedo";
	case Texture::Normal:
		return "Normal";
	case Texture::Metallic:
		return "Metallic";
	case Texture::Roughness:
		return "Roughness";
	case Texture::AmbientOcclusion:
		return "AO";
	default:
		return "Not a texture type";
	}
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

	resolution = glm::ivec2(width, height);

	if (data)
	{
		GLenum format = 0;
		GLenum gammaCorrection = 0;

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

	printf("Trying to load Texture: %s\n", filepath.c_str());
	float *data = stbi_loadf(filepath.c_str(), &width, &height, &nrChannels, 0);

	resolution = glm::ivec2(width, height);

	if (data)
	{
		GenerateTexture();
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);
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
