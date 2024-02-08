#pragma once
#include "Resource.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <string>
#include <stdexcept>

namespace GameEngine
{
	/*
	*  Audio struct inheriting from Resource, holds load and load audio file functions
	*/
	struct Audio : Resource
	{
		~Audio();

		void onLoad() override; // Prepares buffer and source id
		void load_ogg(const std::string&, std::vector<unsigned char>&, ALenum&, ALsizei&); // Loads audio source from file
		
		// Source, buffer Id used when refering to loaded audio
		ALuint m_sourceId;
		ALuint m_bufferId;
	};
}


