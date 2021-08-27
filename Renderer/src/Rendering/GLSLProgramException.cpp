#include "Rendererpch.h"
#include "GLSLProgramException.h"

GLSLProgramException::GLSLProgramException(const std::string& message) :
	std::runtime_error(message)
{
}
