#include "Rendererpch.h"
#include "Shader.h"

#include <gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexCode = getShaderCode(vertexPath.c_str());
	const char* vertexShaderCode = vertexCode.c_str();

	std::string fragmentCode = getShaderCode(fragmentPath.c_str());
	const char* fragmentShaderCode = fragmentCode.c_str();

	//Compile Shaders
	printf("Compiling %s\n", vertexPath.c_str());
	GLuint vertexShader = compileShader(vertexShaderCode, GL_VERTEX_SHADER);

	printf("Compiling %s\n", fragmentPath.c_str());
	GLuint fragmentShader = compileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);

	// Link shaders
	linkShaders(vertexShader, fragmentShader);
}

Shader::Shader(const std::string & vertexPath, const std::string & fragmentPath, const std::string & geometryPath)
{
	std::string vertexCode = getShaderCode(vertexPath.c_str());
	const char* vertexShaderCode = vertexCode.c_str();

	std::string fragmentCode = getShaderCode(fragmentPath.c_str());
	const char* fragmentShaderCode = fragmentCode.c_str();

	std::string geometryCode = getShaderCode(geometryPath.c_str());
	const char* geometryShaderCode = geometryCode.c_str();

	printf("Compiling %s\n", vertexPath.c_str());
	GLuint vertexShader = compileShader(vertexShaderCode, GL_VERTEX_SHADER);

	printf("Compiling %s\n", fragmentPath.c_str());
	GLuint fragmentShader = compileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);

	printf("Compiling %s\n", geometryPath.c_str());
	GLuint geometryShader = compileShader(geometryShaderCode, GL_GEOMETRY_SHADER);

	linkShaders(vertexShader, fragmentShader, geometryShader);

}

Shader::~Shader()
{
	glDeleteProgram(id);
}

void Shader::Use()
{
	glUseProgram(id);
}

void Shader::SetBool(const std::string & name, bool value)
{
	glUniform1i(GetUniformFromCache(name), (int)value);
}

void Shader::SetInt(const std::string & name, int value)
{
	glUniform1i(GetUniformFromCache(name), value);
}

void Shader::SetFloat(const std::string & name, float value)
{
	glUniform1f(GetUniformFromCache(name), value);
}

void Shader::SetVec2(const std::string & name, glm::vec2 value)
{
	glUniform2fv(GetUniformFromCache(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string & name, glm::vec3 value)
{
	glUniform3fv(GetUniformFromCache(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, glm::vec4 value)
{
	glUniform4fv(GetUniformFromCache(name), 1, glm::value_ptr(value));
}

void Shader::SetMat3(const std::string& name, glm::mat3 value)
{
	glUniformMatrix3fv(GetUniformFromCache(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string & name, glm::mat4 value)
{
	glUniformMatrix4fv(GetUniformFromCache(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetSubroutine(const SubroutineParameters & parameters)
{
	glUniformSubroutinesuiv(parameters.Shader, 1, &GetSubroutineIndexFromCache(parameters));
}

const GLuint Shader::GetID() const
{
	return id;
}

std::string Shader::getShaderCode(const char * filePath)
{
	std::string shaderCode;
	std::ifstream shaderFile;

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		shaderFile.open(filePath);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		printf("Error: %s not succesfully read\n", filePath);
	}

	return shaderCode;
}

GLuint Shader::compileShader(const char * shaderCode, GLuint shaderType)
{
	int success;
	char infoLog[512];
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, nullptr);
	glCompileShader(shader);
	//Print compile errors if any
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success)
	{
		printf("SUCCES: Compiled shader\n");
	}
	else
	{
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cout << "Error: " << infoLog << std::endl;
	}
	return shader;
}

void Shader::linkShaders(GLuint vertexShader, GLuint fragmentShader)
{
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);

	glLinkProgram(id);

	//print linking errors if any
	int success;
	char infoLog[512];

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (success)
	{
		printf("SUCCESS: Linked shaders\n");
	}
	else
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "Error: " << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::linkShaders(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader)
{
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glAttachShader(id, geometryShader);

	glLinkProgram(id);

	//print linking errors if any
	int success;
	char infoLog[512];

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (success)
	{
		printf("SUCCESS: Linked shaders\n");
	}
	else
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "Error: " << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);
}

const GLint & Shader::GetUniformFromCache(const std::string & name)
{
	if (uniformCache.count(name) == 0)
	{
		GLint uniformLocation = glGetUniformLocation(id, name.c_str());
		uniformCache[name] = uniformLocation;
		return uniformCache[name];
	}
	return uniformCache[name];
}

const GLuint & Shader::GetSubroutineIndexFromCache(const SubroutineParameters & parameters)
{
	if (subroutineIndexCache.count(parameters.Name) == 0)
	{
		GLuint subroutineLocation = glGetSubroutineIndex(id, parameters.Shader, parameters.Name.c_str());
		subroutineIndexCache[parameters.Name] = subroutineLocation;
		return subroutineIndexCache[parameters.Name];
	}
	return subroutineIndexCache[parameters.Name];
}

Shader::SubroutineParameters::SubroutineParameters(const std::string & name, GLenum shaderType) :
	Name(name), Shader(shaderType)
{
}

bool Shader::SubroutineParameters::operator==(SubroutineParameters & rhs)
{
	return Name == rhs.Name && Shader == rhs.Shader;
}
