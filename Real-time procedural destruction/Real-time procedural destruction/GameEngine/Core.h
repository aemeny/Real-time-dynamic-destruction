#pragma once
#include "Audio.h"
#include "Window.h"
#include "Environment.h"
#include "Resources.h"
#include "Exception.h"
#include "Entity.h"

namespace GameEngine
{
struct Component;
struct TraceRay;
struct Camera;

//!  A Core Struct. 
/*!
  The core of the game engine, this struct hold the main game loop within and is vital
  for the program to run. It contains references to everyother entity and component in the engine.
*/
struct Core
{
	//! Core constructor.
   /*!
	 The default constructor is not used as instead a initalize function is used,
	 this way i can return a shared pointer reference of core to my main to hold on to.
   */
	Core();
	~Core();

	static std::shared_ptr<Core> initialize();

	//! A function variable.
	/*!
	  Main run function thats called after the core and all entities ate initialized,
	  runs updates for inputs and all entities then displays then in a game loop.
	*/
	void run();
	void stop();
	std::shared_ptr<Entity> addEntity();

	//! A find function variable.
	/*!
	  \param _out an vector of shared pointers argument.
	  Find function that returns a vector of any type thats passed into it.
	  Checks through all entities then all their components for given template.
	*/
	template <typename T>
	void find(std::vector<std::shared_ptr<T> >& _out)
	{
		// Go through each Entity in Core.
		for (size_t ei = 0; ei < m_entities.size(); ++ei)
		{
			std::shared_ptr<Entity> e = m_entities.at(ei);
			// Go through each Component in Entity.
			for (size_t ci = 0; ci < e->m_components.size(); ++ci)
			{
				std::shared_ptr<Component> c = e->m_components.at(ci);
				// Try to dynamic cast the Component to a T.
				std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(c);
				// If succeeds then add it to the output array.
				if (t)
				{
					_out.push_back(t);
				}
			}
		}
	}

	void setMainCam(std::weak_ptr<Camera> _cam) { m_mainCam = _cam; }
	std::weak_ptr<Camera> getMainCam() 
	{ 
		if (m_mainCam.expired())
			throw std::runtime_error("Please Assign A Main Camera");
		else
			return m_mainCam;
	}

	//! Public variables.
	/*!
	  Object references and audio initialize variables.
	  bool to check if the game is over, 
	  used to show end screen gui and stop ticking entities
	*/
	std::shared_ptr<Window> m_nativeWindow;
	std::shared_ptr<Resources> m_resources;
	std::shared_ptr<Environment> m_environment;
	std::shared_ptr<Input> m_input;
	std::shared_ptr<TraceRay> m_traceRay;

	ALCdevice* m_device;
	ALCcontext* m_context;

	bool m_gameOver;
private:
	//! Function variables.
   /*!
	 Main program loop running bool
	 vector of all entities
	 reference to itself
   */
	bool m_running;
	std::vector<std::shared_ptr<Entity> > m_entities;
	std::weak_ptr<Camera> m_mainCam;
	std::weak_ptr<Core> m_self;
};

}