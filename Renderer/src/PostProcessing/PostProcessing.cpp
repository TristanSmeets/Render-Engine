#include "Rendererpch.h"
#include "PostProcessing.h"


PostProcessing::PostProcessing()
{
}

PostProcessing::~PostProcessing()
{
}

const PostProcessing::Parameters & PostProcessing::GetParameters() const
{
	return parameters;
}

void PostProcessing::SetParameters(const Parameters & parameters)
{
	this->parameters = parameters;
}
