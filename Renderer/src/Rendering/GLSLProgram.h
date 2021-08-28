#pragma once
#include <glad/glad.h>
#include <glm.hpp>

namespace GLSLShader
{
	enum GLSLShaderType
	{
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER,
		TESS_CONTROL = GL_TESS_CONTROL_SHADER,
		TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
		COMPUTE = GL_COMPUTE_SHADER
	};
}

class GLSLProgram
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
	GLSLProgram(const std::string& vertexPath, const std::string& fragmentPath);
	GLSLProgram(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);
	GLSLProgram();
	~GLSLProgram();

	// Make sure it GLSL programs can not be copied.
	GLSLProgram(const GLSLProgram&) = delete;
	GLSLProgram& operator= (const GLSLProgram&) = delete;

	void CompileShader(const std::string& filename);
	void CompileShader(const std::string& filename, GLSLShader::GLSLShaderType type);
	void CompileShader(const char* source, GLSLShader::GLSLShaderType type, const std::string& filename = std::string());
	void Link();
	void Use() const;
	void Validate() const;
	const GLuint GetHandle() const;
	const bool IsLinked() const;

	void BindAttribLocation(GLuint location, const std::string& name);
	void BindFragDataLocation(GLuint location, const std::string& name);
	
	void SetUniform(const std::string& name, bool value);
	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, unsigned int value);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, glm::vec2 value);
	void SetUniform(const std::string& name, glm::vec3 value);
	void SetUniform(const std::string& name, glm::vec4 value);
	void SetUniform(const std::string& name, glm::mat3 value);
	void SetUniform(const std::string& name, glm::mat4 value);
	void SetSubroutine(const SubroutineParameters& parameters);

	void PrintActiveUniforms();
	void PrintActiveUniformBlocks();
	void PrintActiveAttribs();
	
private:
	std::unordered_map<std::string, GLint> uniformLocationCache;
	std::unordered_map<std::string,GLuint> subroutineIndexCache;
	GLuint programHandle = 0;
	bool linked = false;
	
	void detachAndDeleteShaderObjects();
	bool fileExists(const std::string& filename);
	std::string getExtension(const std::string& filename);
	std::string getShaderCode(const char* filePath);
	GLuint compileShader(const char* shaderCode, GLuint shaderType);
	void linkShaders(GLuint vertexShader, GLuint fragmentShader);
	void linkShaders(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader);
	const GLint& GetUniformLocation(const std::string& name);
	const GLuint& GetSubroutineIndexFromCache(const SubroutineParameters& parameters);

	void findUniformLocations();
	std::string getTypeString(GLenum type);
};
