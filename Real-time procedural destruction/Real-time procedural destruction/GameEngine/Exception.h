#pragma once
#include <exception>
#include <string>

namespace GameEngine
{
	/*
	*  Exception struct inheriting from std::exception, used to handle exceptions thrown
	*/
	struct Exception : public std::exception
	{
		Exception(std::string _message);
		~Exception() throw();

		const char* what(); // What the exception was

	private:
		std::string m_message;
	};
}

