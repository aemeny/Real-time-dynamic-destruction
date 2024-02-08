#pragma once
#include <string>

namespace GameEngine
{
	/*
	*  Resource struct, used to store file paths and load resources
	*/
	struct Resource
	{
	public:
		virtual void onLoad() = 0; // Virtual used by child structs

		std::string getPath() const;
		std::string getPath2() const;

	private:
		friend struct Resources;
		std::string m_path;
		std::string m_path2;

		void load(); // Calls virtual function
	};
}