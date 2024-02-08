#include "AudioListener.h"


namespace GameEngine
{
	/*
   * constructor to set defualt listener position
   */
	AudioListener::AudioListener() : m_position(0) //Default position at 0,0,0
	{
		alListener3f(AL_POSITION, m_position.x, m_position.y, m_position.z);
	}

	/*
   * updates position of listener
   */
	void AudioListener::setPos(glm::vec3 _pos)
	{
		 m_position = _pos; 
		 alListener3f(AL_POSITION, m_position.x, m_position.y, m_position.z);
	}

}
