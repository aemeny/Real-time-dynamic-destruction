#pragma once
#include "Core.h"

namespace GameEngine
{
	//!  Component Struct. 
	/*!
	 the base for adding components to the engine.
	 Used as a parent struct for all components with override functions for
	 tick, initialize, gui display and normal display / rendering
	*/
	struct Component
	{
		//! tick function variable.
		/*!
		  calls the override function onTick() for all child components
		*/
		void tick();

		//! gui function variable.
		/*!
		  calls the override function onGUI() for all child components
		*/
		void gui();

		//! display function variable.
		/*!
		  calls the override function onDisplay() for all child components
		*/
		void display();

		//! get delta time variable function.
		/*!
		  returns the delta time from higher up the hierarchy for a short cut
		*/
		double getDT() const
		{
			return m_entity.lock()->m_core.lock()->m_environment->getDT();
		}

		//! get core variable function.
		/*!
		  returns the core pointer from higher up the hierarchy for a short cut
		*/
		std::weak_ptr<Core> core() const
		{
			return m_entity.lock()->m_core.lock();
		}

		//! get transform variable function.
		/*!
		  returns the transform pointer from higher up the hierarchy for a short cut
		*/
		std::weak_ptr<Transform> transform() const
		{
			return m_entity.lock()->m_transform.lock();
		}

		//! Public variables.
		/*!
		  Object references to the parent entity
		  and a string of the type of component e.g. ModelLoader
		*/
		std::weak_ptr<Entity> m_entity;
		std::string m_type;
	private:
		
		//! A pure virtual member functions.
	  /*!
		 \sa tick()
		 \sa gui()
		 \sa display()
		 \param _input the first argument.
	  */
		virtual void initialize();
		virtual void initialize(std::weak_ptr<Input> _input, float _FOV);
		virtual void onTick();
		virtual void onGUI();
		virtual void onDisplay();
		friend Entity;
	};
}