#pragma once
#include <memory>
#include <string>
#include <Vector>
#include "Input.h"

namespace GameEngine
{
	struct Core;
	struct Component;
	struct Transform;

	//!  Entity class. 
	/*!
	  Entity class is based of unity's entity system,
	  holding a list of components connected to the entity
	  and is used as a parent to hold and manage all components
	*/
	struct Entity
	{
	private:
		friend struct Core;
		
		std::vector<std::shared_ptr<Component>> m_components;
		bool m_alive;
		
	public:
		//! Public variables.
		/*!
		  Object references to core,
		  itself and a transform component
		*/
		std::weak_ptr<Core> m_core;
		std::weak_ptr<Entity> m_self;
		std::weak_ptr<Transform> m_transform;
		
		//! A constructor.
		/*!
		  The constructor is mainly bypassed as we initliaze the values in 
		  core addEntity() function.
		*/
		Entity();
		//! A get function variable.
		/*!
		  returns bool m_alive when called
		*/
		bool getAlive();

		//! display function variable.
		/*!
		  loops through all componenets and calls display on them
		*/
		void display();

		//! kill function variable.
		/*!
		  loops through all componenets and calls kill on them
		*/
		void kill();

		//! tick function variable.
		/*!
		  loops through all componenets and calls tick on them
		*/
		void tick();

		//! gui loading function variable.
		/*!
		  loops through all componenets and calls gui on them to render
		*/
		void gui();

		//! A find function variable.
		/*!
		  Find function that returns a component of any type thats passed into it.
		  Checks through all components in the same entity and returns.
		*/
		template <typename T>
		std::weak_ptr<T> findComponent()
		{
			//search for previosly loaded component
			for (size_t i = 0; i < m_components.size(); ++i)
			{
				//return component if found
				if (m_components.at(i)->m_type == typeid(T).name())
				{
					return std::dynamic_pointer_cast<T>(m_components.at(i));
				}
			}
		}

		//! An add component function.
		/*!
		  adds and initliazes a new component of the type passed,
		  all components have an initialize override so that is called here 
		  in par with setting variables.
		  Once initialized they are added to the component list for this entity
		*/
		template <typename T>
		std::shared_ptr<T> addComponent()
		{
			std::shared_ptr<T> rtn = std::make_shared<T>();

			rtn->m_entity = m_self;
			rtn->m_type = typeid(T).name();
			rtn->initialize();

			if (typeid(T).name() == "Transform")
			{
				m_transform = std::dynamic_pointer_cast<Transform>(rtn);
			}
			m_components.push_back(rtn);

			return rtn;
		}

		//! An add component function (polymorphism)
		/*!
		  used the same way as the other function but takes in an input
		  object reference & FOV value to be passed to the initialize function
		*/
		template <typename T>
		std::shared_ptr<T> addComponent(std::shared_ptr<Input> _input, float _FOV)
		{
			std::shared_ptr<T> rtn = std::make_shared<T>();

			rtn->m_entity = m_self;
			rtn->m_type = typeid(T).name();
			rtn->initialize(_input, _FOV);

			m_components.push_back(rtn);

			return rtn;
		}
	};

}