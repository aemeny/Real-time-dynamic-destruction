#pragma once
#include "Component.h"

namespace GameEngine
{
	/*
	*  AudioSource struct inheriting from component, hold init, tic and start audio, used to play audio from a world position
	*/
	struct AudioSource : Component
	{
		void initialize() override; 
		void onTick() override; // Checks if audio is still playing

		void setPosition(glm::vec3 _pos) { m_position = _pos; } // Setter for position
		void setAudio(std::shared_ptr<Audio> _audio); // Setter for audio source
		void startAudio(); // Plays the audio resource loaded

	private:
		glm::vec3 m_position;

		std::weak_ptr<Audio> m_audio;

		bool m_audioCheck;
	};
}

