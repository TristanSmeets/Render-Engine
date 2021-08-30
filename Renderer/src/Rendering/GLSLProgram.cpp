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

GLSLProgram::GLSLProgram()
= default;

GLSLProgram::~GLSLProgram()
{
	if(programHandle == 0)
	{
		return;
	}

	DetachAndDeleteShaderObjects();
	glDeleteProgram(programHandle);
}

void GLSLProgram::CompileShader(const std::string& filename)
{
	std::string extension = GetExtension(filename);
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
	if(!FileExists(filename))
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

void GLSLProgram::Link()
{
	if(linked)
	{
		return;
	}
	if(programHandle <=0 )
	{
		throw GLSLProgramException("Program has not been compiled.");
	}

	glLinkProgram(programHandle);

	// Check for errors
	int status = 0;

	glGetProgramiv(programHandle, GL_LINK_STATUS, &status);

	if(GL_FALSE == status)
	{
		int length = 0;
		glGetShaderiv(programHandle, GL_INFO_LOG_LENGTH, &length);
		std::string errorMessage = "Program link failed:\n";
		
		if (length > 0)
		{
			std::string log(length, ' ');
			int written = 0;
			glGetProgramInfoLog(programHandle, length, &written, &log[0]);
			errorMessage += log;
		}
		throw GLSLProgramException(errorMessage);
	}
	FindUniformLocations();
	linked = true;
	DetachAndDeleteShaderObjects();
	
}

void GLSLProgram::Use() const
{
	glUseProgram(programHandle);
}

void GLSLProgram::Validate() const
{
	if (!IsLinked())
	{
		throw GLSLProgramException("Program is not linked");
	}

	GLint status;
	glValidateProgram(programHandle);
	glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &status);

	if (GL_FALSE == status) {
		// Store log and return false
		int length = 0;
		std::string logString;

		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &length);

		if (length > 0) {
			char* c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog(programHandle, length, &written, c_log);
			logString = c_log;
			delete[] c_log;
		}

		throw GLSLProgramException(std::string("Program failed to validate\n") + logString);
	}
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

void GLSLProgram::FindUniformLocations()
{
	uniformLocationCache.clear();
	GLint numberOfUniforms = 0;

	glGetProgramInterfaceiv(programHandle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numberOfUniforms);
	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

	for(GLint i = 0; i < numberOfUniforms; ++i)
	{
		GLint results[4];

		glGetProgramResourceiv(programHandle, GL_UNIFORM, i, 4, properties, 4, nullptr, results);

		if(results[3] != -1)
		{
			continue;
		}

		GLint nameBufferSize = results[0] + 1;
		char* name = new char[nameBufferSize];
		glGetProgramResourceName(programHandle, GL_UNIFORM, i, nameBufferSize, nullptr, name);
		uniformLocationCache[name] = results[2];
		delete[] name;
	}
}

void GLSLProgram::PrintActiveUniforms()
{
	GLint numUniforms = 0;
	glGetProgramInterfaceiv(programHandle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

	printf("Active uniforms:\n");
	for (int i = 0; i < numUniforms; ++i) {
		GLint results[4];
		glGetProgramResourceiv(programHandle, GL_UNIFORM, i, 4, properties, 4, NULL, results);

		if (results[3] != -1) continue;  // Skip uniforms in blocks
		GLint nameBufSize = results[0] + 1;
		char* name = new char[nameBufSize];
		glGetProgramResourceName(programHandle, GL_UNIFORM, i, nameBufSize, NULL, name);
		printf("%-5d %s (%s)\n", results[2], name, GetTypeString(results[1]).c_str());
		delete[] name;
	}
}

void GLSLProgram::PrintActiveUniformBlocks()
{
	GLint numBlocks = 0;

	glGetProgramInterfaceiv(programHandle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
	GLenum blockProps[] = { GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH };
	GLenum blockIndex[] = { GL_ACTIVE_VARIABLES };
	GLenum props[] = { GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX };

	for (int block = 0; block < numBlocks; ++block) {
		GLint blockInfo[2];
		glGetProgramResourceiv(programHandle, GL_UNIFORM_BLOCK, block, 2, blockProps, 2, NULL, blockInfo);
		GLint numUnis = blockInfo[0];

		char* blockName = new char[blockInfo[1] + 1];
		glGetProgramResourceName(programHandle, GL_UNIFORM_BLOCK, block, blockInfo[1] + 1, NULL, blockName);
		printf("Uniform block \"%s\":\n", blockName);
		delete[] blockName;

		GLint* unifIndexes = new GLint[numUnis];
		glGetProgramResourceiv(programHandle, GL_UNIFORM_BLOCK, block, 1, blockIndex, numUnis, NULL, unifIndexes);

		for (int unif = 0; unif < numUnis; ++unif) {
			GLint uniIndex = unifIndexes[unif];
			GLint results[3];
			glGetProgramResourceiv(programHandle, GL_UNIFORM, uniIndex, 3, props, 3, NULL, results);

			GLint nameBufSize = results[0] + 1;
			char* name = new char[nameBufSize];
			glGetProgramResourceName(programHandle, GL_UNIFORM, uniIndex, nameBufSize, NULL, name);
			printf("    %s (%s)\n", name, GetTypeString(results[1]).c_str());
			delete[] name;
		}

		delete[] unifIndexes;
	}
}

void GLSLProgram::PrintActiveAttribs()
{
	GLint numberOfAttribs;
	glGetProgramInterfaceiv(programHandle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numberOfAttribs);

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

	printf("Active attributes:\n");
	for (int i = 0; i < numberOfAttribs; ++i) {
		GLint results[3];
		glGetProgramResourceiv(programHandle, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

		GLint nameBufSize = results[0] + 1;
		char* name = new char[nameBufSize];
		glGetProgramResourceName(programHandle, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);
		printf("%-5d %s (%s)\n", results[2], name, GetTypeString(results[1]).c_str());
		delete[] name;
	}
}

std::string GLSLProgram::GetTypeString(GLenum type)
{
	switch (type) {
	case GL_FLOAT:
		return "float";
	case GL_FLOAT_VEC2:
		return "vec2";
	case GL_FLOAT_VEC3:
		return "vec3";
	case GL_FLOAT_VEC4:
		return "vec4";
	case GL_DOUBLE:
		return "double";
	case GL_INT:
		return "int";
	case GL_UNSIGNED_INT:
		return "unsigned int";
	case GL_BOOL:
		return "bool";
	case GL_FLOAT_MAT2:
		return "mat2";
	case GL_FLOAT_MAT3:
		return "mat3";
	case GL_FLOAT_MAT4:
		return "mat4";
	default:
		return "?";
	}
}

const GLuint GLSLProgram::GetHandle() const
{
	return programHandle;
}

const bool GLSLProgram::IsLinked() const
{
	return linked;
}

void GLSLProgram::BindAttribLocation(GLuint location, const std::string& name)
{
	glBindAttribLocation(programHandle, location, name.c_str());
}

void GLSLProgram::BindFragDataLocation(GLuint location, const std::string& name)
{
	glBindFragDataLocation(programHandle, location, name.c_str());
}

void GLSLProgram::DetachAndDeleteShaderObjects()
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

bool GLSLProgram::FileExists(const std::string& filename)
{
	struct stat info;
	int ret = -1;

	ret = stat(filename.c_str(), &info);
	return 0 == ret;
}

std::string GLSLProgram::GetExtension(const std::string& filename)
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

std::string GLSLProgram::GetShaderCode(const char * filePath)
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
