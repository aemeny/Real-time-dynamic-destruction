#include "Core.h"
#include "..\Physics\TraceRay.h"

namespace GameEngine
{
	Core::Core() {}
	/// Clean up
	Core::~Core() {
		alcMakeContextCurrent(NULL);
		alcDestroyContext(m_context);
		alcCloseDevice(m_device);
	}

	/// Main game loop.
	/** Run fuction incapsulated the core of the game engine with the main game loop, ticks, displays and gets inputs*/
	void Core::run()
	{
		m_running = true;
		m_gameOver = false;

		m_environment = std::make_shared<Environment>(m_self);

		double fpsTimer = 0;
		int ticks = 0;
		while (m_running) // Main loop
		{
			//Clear depth buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//draw background
			glClearColor(0.725f, 0.36f, 0.956f, 1.0f);

			// Call tick
			m_environment->tick();

			m_input->tick();

			if (m_input->isKeyPressed(SDLK_ESCAPE)) // When escape is pressed break out of game loop
			{
				break;
			}

			if (!m_gameOver) // If the game is lost stop ticking entitys
			{
				for (size_t ei = 0; ei < m_entities.size(); ++ei)
				{
					m_entities[ei]->tick();
				}

				// Remove "killed" entities
				for (size_t ei = 0; ei < m_entities.size(); ++ei)
				{
					if (!m_entities[ei]->getAlive())
					{
						m_entities.at(ei)->kill();
						m_entities.erase(m_entities.begin() + ei);
						--ei;
					}
				}
			}
			// Call display on all entities
			for (size_t ei = 0; ei < m_entities.size(); ++ei)
			{
				m_entities.at(ei)->display();
			}
			m_traceRay->display();

			// Call gui on all entities
			for (size_t ei = 0; ei < m_entities.size(); ++ei)
			{
				m_entities.at(ei)->gui();
			}

			if (m_input->isKeyPressed(SDLK_e)) // When escape is pressed break out of game loop
			{
				m_traceRay->stepDebugDrawBox();
			}

			//Clear pressed/released keys for this frame
			m_input->clear();

			//Swap window
			SDL_GL_SwapWindow(m_nativeWindow->m_window);

			// Display FPS
			if (fpsTimer >= 1)
			{
				//std::cout << "FPS: " << ticks << std::endl;
				fpsTimer = 0;
				ticks = 0;
			}
			else
			{
				fpsTimer += m_environment->getDT();
				ticks++;
			}
		}
	}

	void Core::stop() // Stops the main loop
	{
		m_running = false;
	}

	/// Core initialize.
	/** Returns the shared pointer from a fully made core object where the SDL Window, AL audio, glew and other objects are all initliazed */
	std::shared_ptr<Core> Core::initialize()
	{
		std::shared_ptr<Core> rtn = std::make_shared<Core>();

		//Init variables
		rtn->m_input = std::make_shared<Input>(rtn);

		rtn->m_resources = std::make_shared<Resources>();

		rtn->m_traceRay = std::make_shared<TraceRay>(rtn);

		rtn->m_nativeWindow = std::make_shared<Window>();
		rtn->m_nativeWindow->m_windowHeight = 1000;
		rtn->m_nativeWindow->m_windowWidth = 1000;

		rtn->m_self = rtn;


		// Window Initialise
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			throw std::runtime_error("Failed to initialize SDL");
		}

		if (!(rtn->m_nativeWindow->m_window = SDL_CreateWindow("WINDOW",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			rtn->m_nativeWindow->m_windowWidth, rtn->m_nativeWindow->m_windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)))
		{
			SDL_Quit();
			throw std::runtime_error("Failed to create Window");
		}

		if (!(rtn->m_nativeWindow->m_context = SDL_GL_CreateContext(rtn->m_nativeWindow->m_window)))
		{
			SDL_DestroyWindow(rtn->m_nativeWindow->m_window);
			SDL_Quit();
			throw std::runtime_error("Failed to create OpenGL context");
		}

		// Audio Initialise
		rtn->m_device = alcOpenDevice(NULL);

		if (!rtn->m_device)
		{
			throw std::runtime_error("Failed to open audio device");
		}

		rtn->m_context = alcCreateContext(rtn->m_device, NULL);

		if (!rtn->m_context)
		{
			alcCloseDevice(rtn->m_device);
			throw std::runtime_error("Failed to create audio context");
		}

		if (!alcMakeContextCurrent(rtn->m_context))
		{
			alcDestroyContext(rtn->m_context);
			alcCloseDevice(rtn->m_device);
			throw std::runtime_error("Failed to make context current");
		}

		// Turn vsync on for SDL
		SDL_GL_SetSwapInterval(1);

		// Turn off cursor
		SDL_ShowCursor(false);

		// Init glew
		glewInit();

		return rtn;
	}
	
	// Creates a new entity and adds it the the vector
	std::shared_ptr<Entity> Core::addEntity()
	{
		std::shared_ptr<Entity> rtn = std::make_shared<Entity>();

		rtn->m_core = m_self;
		rtn->m_self = rtn;
		rtn->m_alive = true;

		m_entities.push_back(rtn);

		return rtn;
	}

}