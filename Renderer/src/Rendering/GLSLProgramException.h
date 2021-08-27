#pragma once
#include <stdexcept>

class GLSLProgramException : std::runtime_error
{
public:
	GLSLProgramException(const std::string& message);
};
