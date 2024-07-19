#pragma once
#include <memory>
#include <vector>
#include "Shader.h"

namespace GameEngine
{
	/*
	*  Resources struct, used to load and initialize components
	*/
	struct Resources
	{
	public:
		Resources();

		template <typename T>
		std::shared_ptr<T> load(bool _modifiable, const std::string& _path, const std::string& _path2 = "")
		{
			if (!_modifiable)
			{
				//search for previosly loaded resource
				for (size_t i = 0; i < m_resources.size(); ++i) // check through resources 
				{
					//return resource if found
					if (m_resources.at(i)->getPath() == _path) // if the resource path already exists
					{
						return std::dynamic_pointer_cast<T>(m_resources.at(i)); //return the found resource
					}
				}
			}

			//create new instance, construct and add to cache
			std::shared_ptr<T> rtn = std::make_shared<T>();
			rtn->m_path = _path;
			rtn->m_path2 = _path2;

			rtn->load();
			m_resources.push_back(rtn);

			return rtn;
		}

	private:
		std::vector<std::shared_ptr<Resource> > m_resources;
	};
}