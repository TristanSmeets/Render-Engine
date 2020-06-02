#pragma once
#include <glad/glad.h>
#include <glm.hpp>

class Shader
{
public:
	struct SubroutineParameters
	{
		std::string Name;
		GLenum Shader;

		SubroutineParameters(const std::string& name = "Default", GLenum shaderType = GL_FRAGMENT_SHADER);
		bool operator==(SubroutineParameters& parameters);
	};

public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);
	~Shader();
	void Use();
	void SetBool(const std::string& name, bool value);
	void SetInt(const std::string& name, int value);
	void SetFloat(const std::string& name, float value);
	void SetVec3(const std::string& name, glm::vec3 value);
	void SetMat4(const std::string& name, glm::mat4 value);
	void SetSubroutine(const SubroutineParameters& parameters);
	const GLuint GetID() const;

private:
	std::string getShaderCode(const char* filePath);
	GLuint compileShader(const char* shaderCode, GLuint shaderType);
	void linkShaders(GLuint vertexShader, GLuint fragmentShader);
	void linkShaders(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader);
	const GLint& GetUniformFromCache(const std::string& name);
	const GLuint& GetSubroutineIndexFromCache(const SubroutineParameters& parameters);

	std::unordered_map<std::string, GLint> uniformCache;
	std::unordered_map<std::string,GLuint> subroutineIndexCache;
	GLuint id = 0;

};
