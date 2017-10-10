#include "AudioSystem.h"
#include "EntityWorld.h"
#include "EventSystem.h"

#include <algorithm>

namespace audio
{
	AudioSystem::AudioSystem(component::EntityWorld& entityWorld)
		: m_entityWorld(entityWorld)
	{
		using namespace events;
		EventSystem::registerListener(EventType::PlaySound, [this](EventData* eventData) { onPlaySound(eventData); });
	}

	void AudioSystem::start()
	{
	}

	void AudioSystem::update(float deltaTime)
	{
		for (auto& musicComp : m_entityWorld.getMusicWorld().getArray())
		{
			if (musicComp.isPlaying && m_playedMusic.count(musicComp.componentId) == 0)
			{
				//Start playing music
				m_playedMusic[musicComp.componentId].openFromFile(musicComp.filePath);
				m_playedMusic[musicComp.componentId].setLoop(musicComp.isLooping);
				m_playedMusic[musicComp.componentId].play();
			}
			else if (!musicComp.isPlaying && m_playedMusic.count(musicComp.componentId) > 0)
			{
				//Stop playing music
				m_playedMusic[musicComp.componentId].stop();
				m_playedMusic.erase(musicComp.componentId);
			}
			else if (musicComp.isPlaying && m_playedMusic.count(musicComp.componentId) > 0 && m_playedMusic[musicComp.componentId].getStatus() == sf::Music::Stopped)
			{
				//Music finished playing
				musicComp.isPlaying = false;
				m_playedMusic.erase(musicComp.componentId);
			}
		}

		//Remove finished sounds
		std::remove_if(m_playedSounds.begin(), m_playedSounds.end(), [this](const sf::Sound& sound)
		{
			return sound.getStatus() == sf::Sound::Stopped;
		});
	}

	void AudioSystem::onPlaySound(events::EventData* eventData)
	{
		using namespace events;

		if (PlaySoundData* data = static_cast<PlaySoundData*>(eventData))
		{
			Sound* sound = m_soundLoader.load(data->path);

			//Load if first time playing
			if (!sound->isLoaded && sound->buffer.loadFromFile(sound->path))
			{
				sound->isLoaded = true;
			}

			if (sound->isLoaded)
			{
				m_playedSounds.push_front(sf::Sound());
				m_playedSounds.front().setBuffer(sound->buffer);
				m_playedSounds.front().play();
			}
		}
	}
}