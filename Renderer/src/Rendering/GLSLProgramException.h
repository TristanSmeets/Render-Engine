#pragma once
#include <stdexcept>

class GLSLProgramException : public std::runtime_error
{
public:
	GLSLProgramException(const std::string& message);
};
