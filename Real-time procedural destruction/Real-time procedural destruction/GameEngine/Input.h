#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>

namespace GameEngine
{
	struct Core;

	/*
	*  Input struct, used check keyboard form both mouse, keyboard and controller, stores input for inputs this frame and held
	*/
	struct Input
	{
		Input(std::weak_ptr<Core> _core);
		~Input();

		bool isKey(int _key); // Is a key held down
		bool isKeyPressed(int _key); // Has a key been pressed this frame
		bool isKeyReleased(int _key); // Has a key been released this frame

		glm::ivec2 getMousePos() { return m_mousePos; } // Getter for mouse cooridnate on the screen
		glm::vec2 getMouseDelta() { return m_mouseDelta; } // Getter for the mouse velocity as it moves around
		bool isMouseDown() { return m_mouseDown; } // If the mouse button is down
		bool isMouseClicked() { return m_mouseDownClick; } // If the mouse button was clicked this frame

		void tick();
		void clear(); // Clear frame specific variables

		bool m_lockMouse; // If the mouse is locked to the middle of the screen
	private:
		SDL_Event m_event;
		std::weak_ptr<Core> m_core;

		// Keyboard input
		std::vector<int> m_keys;
		std::vector<int> m_pressedKeys;
		std::vector<int> m_releasedKeys;

		// Mouse Input
		glm::ivec2 m_mousePos;
		glm::vec2 m_mouseDelta;
		bool m_mouseDown;
		bool m_mouseDownClick;

		/* referenced code for controller SDL support
		*  https://lazyfoo.net/tutorials/SDL/19_gamepads_and_joysticks/index.php
		*/
		// Controller Input
		SDL_GameController* m_controller;
		const int JOYSTICK_DEAD_ZONE = 8000; //Analog joystick dead zone

		//Input init to stop mouse flying at start
		int m_mset;
		bool m_dm;
	};
}

