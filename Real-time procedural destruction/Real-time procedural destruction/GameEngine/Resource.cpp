#include "Resource.h"

namespace GameEngine
{

	std::string Resource::getPath() const
	{
		return m_path;
	}
	
	std::string Resource::getPath2() const
	{
		return m_path2;
	}

	void Resource::load()
	{
		onLoad();
	}

}
