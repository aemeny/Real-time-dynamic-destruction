#pragma once
#include "Audio.h"
#include <stdexcept>

#define STB_VORBIS_HEADER_ONLY 
#include "stb_vorbis.c"

namespace GameEngine
{
    /*
    * Audio clean up
    */
    Audio::~Audio()
    {
        alDeleteSources(1, &m_sourceId);
        alDeleteBuffers(1, &m_bufferId);
    }

    /*
    * onLoad function creats buffer and source id, calls for audio file to be loaded
    */
    void Audio::onLoad()
    {
        /*************************************************************************
        * Preparing buffer
        *************************************************************************/
        ALenum format = 0;
        ALsizei freq = 0;
        std::vector<unsigned char> bufferData;
        load_ogg(getPath().c_str(), bufferData, format, freq);

        m_bufferId = 0;
        alGenBuffers(1, &m_bufferId);

        alBufferData(m_bufferId, format, &bufferData.at(0),
            static_cast<ALsizei>(bufferData.size()), freq);

        /*************************************************************************
         * Preparing sound source
         *************************************************************************/
        m_sourceId = 0;
        alGenSources(1, &m_sourceId);
        
        alSourcei(m_sourceId, AL_BUFFER, m_bufferId);
        //alSourcef(m_sourceId, AL_PITCH, 10);
        //alSourcef(m_sourceId, AL_GAIN, vol);
    }

    /*
    * load_ogg function pulls data from a audio source
    */
    void Audio::load_ogg(const std::string& _path, std::vector<unsigned char>& _buffer,
        ALenum& _format, ALsizei& _freq)
    {
        int channels = 0;
        int sampleRate = 0;
        short* output = NULL;

        size_t samples = stb_vorbis_decode_filename(_path.c_str(),
            &channels, &sampleRate, &output);

        if (samples == -1) // check if its an audio source
        {
            throw std::runtime_error("Failed to open file '" + _path + "' for decoding");
        }

        // Record the format required by OpenAL
        if (channels < 2) 
        {
            _format = AL_FORMAT_MONO16;
        }
        else
        {
            _format = AL_FORMAT_STEREO16;
        }

        // Copy (# samples) * (1 or 2 channels) * (16 bits == 2 bytes == short)
        _buffer.resize(samples * channels * sizeof(short));
        memcpy(&_buffer.at(0), output, _buffer.size());

        // Record the sample rate required by OpenAL
        _freq = sampleRate;

        // Clean up the read data
        free(output);
    }
}