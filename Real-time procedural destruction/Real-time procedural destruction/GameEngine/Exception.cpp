#include "Exception.h"

namespace GameEngine
{
	Exception::Exception(std::string _message)
	{
	}

	Exception::~Exception() throw() {}

	const char* Exception::what()
	{
		return nullptr;
	}

}