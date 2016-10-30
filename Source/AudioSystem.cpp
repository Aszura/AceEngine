#include "AudioSystem.h"
#include "EntityWorld.h"
#include "EventSystem.h"

#include <algorithm>

namespace audio
{
	AudioSystem::AudioSystem(component::EntityWorld* entityWorld)
		: mEntityWorld(entityWorld)
	{
		assert(entityWorld);

		using namespace events;
		EventSystem::registerListener(EventType::PlaySound, [this](EventData* eventData) { onPlaySound(eventData); });
	}

	AudioSystem::~AudioSystem()
	{

	}

	void AudioSystem::start()
	{
	}

	void AudioSystem::update(float deltaTime)
	{
		for (auto& musicComp : mEntityWorld->getMusicWorld().getArray())
		{
			if (musicComp.isPlaying && mPlayedMusic.count(musicComp.componentId) == 0)
			{
				//Start playing music
				mPlayedMusic[musicComp.componentId].openFromFile(musicComp.filePath);
				mPlayedMusic[musicComp.componentId].setLoop(musicComp.isLooping);
				mPlayedMusic[musicComp.componentId].play();
			}
			else if (!musicComp.isPlaying && mPlayedMusic.count(musicComp.componentId) > 0)
			{
				//Stop playing music
				mPlayedMusic[musicComp.componentId].stop();
				mPlayedMusic.erase(musicComp.componentId);
			}
			else if (musicComp.isPlaying && mPlayedMusic.count(musicComp.componentId) > 0 && mPlayedMusic[musicComp.componentId].getStatus() == sf::Music::Stopped)
			{
				//Music finished playing
				musicComp.isPlaying = false;
				mPlayedMusic.erase(musicComp.componentId);
			}
		}

		//Remove finished sounds
		std::remove_if(mPlayedSounds.begin(), mPlayedSounds.end(), [this](const sf::Sound& sound)
		{
			return sound.getStatus() == sf::Sound::Stopped;
		});
	}

	void AudioSystem::onPlaySound(events::EventData* eventData)
	{
		using namespace events;

		if (PlaySoundData* data = static_cast<PlaySoundData*>(eventData))
		{
			Sound* sound = mSoundLoader.load(data->path);

			//Load if first time playing
			if (!sound->isLoaded && sound->buffer.loadFromFile(sound->path))
			{
				sound->isLoaded = true;
			}

			if (sound->isLoaded)
			{
				mPlayedSounds.push_front(sf::Sound());
				mPlayedSounds.front().setBuffer(sound->buffer);
				mPlayedSounds.front().play();
			}
		}
	}
}