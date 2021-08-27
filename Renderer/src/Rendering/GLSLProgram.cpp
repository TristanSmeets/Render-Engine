#include "Rendererpch.h"
#include "GLSLProgram.h"

#include <gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include "GLSLProgramException.h"

namespace GLSLShaderInfo {
	std::map<std::string, GLSLShader::GLSLShaderType> extensions = {
		{".vs",   GLSLShader::VERTEX},
		{".vert", GLSLShader::VERTEX},
		{"_vert.glsl", GLSLShader::VERTEX},
		{".vert.glsl", GLSLShader::VERTEX },
		{".gs",   GLSLShader::GEOMETRY},
		{".geom", GLSLShader::GEOMETRY},
		{ ".geom.glsl", GLSLShader::GEOMETRY },
		{".tcs",  GLSLShader::TESS_CONTROL},
		{ ".tcs.glsl",  GLSLShader::TESS_CONTROL },
		{ ".tes",  GLSLShader::TESS_EVALUATION },
		{".tes.glsl",  GLSLShader::TESS_EVALUATION},
		{".fs",   GLSLShader::FRAGMENT},
		{".frag", GLSLShader::FRAGMENT},
		{"_frag.glsl", GLSLShader::FRAGMENT},
		{".frag.glsl", GLSLShader::FRAGMENT},
		{".cs",   GLSLShader::COMPUTE},
		{ ".cs.glsl",   GLSLShader::COMPUTE }
	};
}

GLSLProgram::GLSLProgram(const std::string& vertexPath, const std::string& fragmentPath)
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

GLSLProgram::GLSLProgram(const std::string & vertexPath, const std::string & fragmentPath, const std::string & geometryPath)
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

GLSLProgram::GLSLProgram()
{
}

GLSLProgram::~GLSLProgram()
{
	if(programHandle == 0)
	{
		return;
	}

	detachAndDeleteShaderObjects();
	glDeleteProgram(programHandle);
}

void GLSLProgram::CompileShader(const std::string& filename)
{
	std::string extension = getExtension(filename);
	GLSLShader::GLSLShaderType type = GLSLShader::VERTEX;
	auto iterator = GLSLShaderInfo::extensions.find(extension);

	if( iterator == GLSLShaderInfo::extensions.end())
	{
		std::string message = "Unrecognized extension: " + extension;
		throw GLSLProgramException(message);		
	}
	
	type = iterator->second;
	CompileShader(filename, type);
}

void GLSLProgram::CompileShader(const std::string& filename, GLSLShader::GLSLShaderType type)
{
	if(!fileExists(filename))
	{
		std::string message = std::string("Shader: ") + filename + " not found.";
		throw GLSLProgramException(message);
	}

	if(programHandle <= 0)
	{
		programHandle = glCreateProgram();

		if(programHandle == 0)
		{
			throw GLSLProgramException("Unable to create shader program.");
		}
	}

	std::ifstream inFile(filename, std::ios::in);

	if(!inFile)
	{
		std::string message = std::string("Unable to open: ") + filename;
		throw GLSLProgramException(message);
	}

	// Get file content
	std::stringstream code;
	code << inFile.rdbuf();
	inFile.close();
	CompileShader(code.str().c_str(), type, filename);
}

void GLSLProgram::CompileShader(const char* source, GLSLShader::GLSLShaderType type, const std::string& filename)
{
	if(programHandle <= 0)
	{
		programHandle = glCreateProgram();
		if(programHandle == 0)
		{
			throw GLSLProgramException("Unable to create shader program.");
		}
	}

	GLuint shaderHandle = glCreateShader(type);
	glShaderSource(shaderHandle, 1, &source, nullptr);

	// Compile the shader
	glCompileShader(shaderHandle);

	// Check for errors
	int result;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
	if(GL_FALSE == result)
	{
		// Compilation failed
		std::string message;
		if(filename.empty())
		{
			message = "Shader compilation failed.\n";
		}
		else
		{
			message = std::string(filename) + ": shader compilation failed.\n";
		}

		int length = 0;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
		if(length > 0)
		{
			std::string log(length, ' ');
			int written = 0;
			glGetShaderInfoLog(shaderHandle, length, &written, &log[0]);
			message += log;
		}
		throw GLSLProgramException(message);
	}
	glAttachShader(programHandle, shaderHandle);
}

void GLSLProgram::Use()
{
	glUseProgram(programHandle);
}

void GLSLProgram::SetUniform(const std::string & name, bool value)
{
	glUniform1i(GetUniformLocation(name), (int)value);
}

void GLSLProgram::SetUniform(const std::string & name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void GLSLProgram::SetUniform(const std::string& name, unsigned int value)
{
	SetUniform(name, static_cast<int>(value));
}

void GLSLProgram::SetUniform(const std::string & name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void GLSLProgram::SetUniform(const std::string & name, glm::vec2 value)
{
	glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void GLSLProgram::SetUniform(const std::string & name, glm::vec3 value)
{
	glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void GLSLProgram::SetUniform(const std::string& name, glm::vec4 value)
{
	glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void GLSLProgram::SetUniform(const std::string& name, glm::mat3 value)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void GLSLProgram::SetUniform(const std::string & name, glm::mat4 value)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void GLSLProgram::SetSubroutine(const SubroutineParameters & parameters)
{
	glUniformSubroutinesuiv(parameters.Shader, 1, &GetSubroutineIndexFromCache(parameters));
}

const GLuint GLSLProgram::GetHandle() const
{
	return programHandle;
}

const bool GLSLProgram::IsLinked() const
{
	return linked;
}

void GLSLProgram::detachAndDeleteShaderObjects()
{
	GLint numberOfShaders = 0;

	glGetProgramiv(programHandle, GL_ATTACHED_SHADERS, &numberOfShaders);

	std::vector<GLuint> shaderNames(numberOfShaders);
	glGetAttachedShaders(programHandle, numberOfShaders, nullptr, shaderNames.data());
	for (GLuint shader : shaderNames)
	{
		glDetachShader(programHandle, shader);
		glDeleteShader(shader);
	}
	
}

bool GLSLProgram::fileExists(const std::string& filename)
{
	struct stat info;
	int ret = -1;

	ret = stat(filename.c_str(), &info);
	return 0 == ret;
}

std::string GLSLProgram::getExtension(const std::string& filename)
{
	size_t dotLocation = filename.find_last_of('.');
	if(dotLocation != std::string::npos)
	{
		std::string ext = filename.substr(dotLocation);

		if(ext == ".glsl")
		{
			size_t location = filename.find_last_of('.', dotLocation - 1);
			if (location == std::string::npos)
			{
				location = filename.find_last_of('_', dotLocation - 1);
			}
			if (location != std::string::npos)
			{
				return filename.substr(location);
			}
		}
		else
		{
			return ext;
		}
	}
	return std::string();
}

std::string GLSLProgram::getShaderCode(const char * filePath)
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

GLuint GLSLProgram::compileShader(const char * shaderCode, GLuint shaderType)
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

void GLSLProgram::linkShaders(GLuint vertexShader, GLuint fragmentShader)
{
	programHandle = glCreateProgram();
	glAttachShader(programHandle, vertexShader);
	glAttachShader(programHandle, fragmentShader);

	glLinkProgram(programHandle);

	//print linking errors if any
	int success;
	char infoLog[512];

	glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
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

void GLSLProgram::linkShaders(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader)
{
	programHandle = glCreateProgram();
	glAttachShader(programHandle, vertexShader);
	glAttachShader(programHandle, fragmentShader);
	glAttachShader(programHandle, geometryShader);

	glLinkProgram(programHandle);

	//print linking errors if any
	int success;
	char infoLog[512];

	glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
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

const GLint & GLSLProgram::GetUniformLocation(const std::string & name)
{
	if (uniformLocationCache.count(name) == 0)
	{
		GLint uniformLocation = glGetUniformLocation(programHandle, name.c_str());
		uniformLocationCache[name] = uniformLocation;
		return uniformLocationCache[name];
	}
	return uniformLocationCache[name];
}

const GLuint & GLSLProgram::GetSubroutineIndexFromCache(const SubroutineParameters & parameters)
{
	if (subroutineIndexCache.count(parameters.Name) == 0)
	{
		GLuint subroutineLocation = glGetSubroutineIndex(programHandle, parameters.Shader, parameters.Name.c_str());
		subroutineIndexCache[parameters.Name] = subroutineLocation;
		return subroutineIndexCache[parameters.Name];
	}
	return subroutineIndexCache[parameters.Name];
}

GLSLProgram::SubroutineParameters::SubroutineParameters(const std::string & name, GLenum shaderType) :
	Name(name), Shader(shaderType)
{
}

bool GLSLProgram::SubroutineParameters::operator==(SubroutineParameters & rhs)
{
	return Name == rhs.Name && Shader == rhs.Shader;
}
