#include "AudioSource.h"
#include "Transform.h"

namespace GameEngine
{
    /*
   * constructor
   */
	void AudioSource::initialize()
	{
        m_audioCheck = true;
        m_position = glm::vec3(0);
	}

    /*
   * onTick() function to check if audio is still running and update the audio source position
   */
	void AudioSource::onTick()
	{
        m_position = transform().lock()->getPos();
        alSource3f(m_audio.lock()->m_sourceId, AL_POSITION, m_position.x, m_position.y, m_position.z);

        // Check state of audio / is running
        int state = 0;
        alGetSourcei(m_audio.lock()->m_sourceId, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING && m_audioCheck)
        {
            m_audioCheck = false;
            std::cout << "AUDIO STOPPED" << std::endl;
        }
	}

    void AudioSource::setAudio(std::shared_ptr<Audio> _audio)
    {
        m_audio = _audio;
    }

    /*
   * Starts the audio source playing
   */
    void AudioSource::startAudio()
    {
        alSourcePlay(m_audio.lock()->m_sourceId);
    }

}