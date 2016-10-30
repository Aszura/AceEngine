#pragma once
#include "CustomTypes.h"
#include "ResourceLoader.h"
#include "Sound.h"

#include <glm\glm.hpp>
#include <map>
#include <forward_list>
#include <SFML\Audio.hpp>

namespace events
{
	struct EventData;
}

namespace component
{
	class EntityWorld;
}

namespace audio
{
	class AudioSystem
	{
	public:
		AudioSystem(component::EntityWorld* entityWorld);
		~AudioSystem();

		void start();
		void update(float deltaTime);
	private:
		void onPlaySound(events::EventData* eventData);

		utility::ResourceLoader<Sound>				mSoundLoader;
		glm::ivec2									mMovementAxis;
		component::EntityWorld*						mEntityWorld;
		std::forward_list<sf::Sound>				mPlayedSounds;
		std::map<component::ElementId, sf::Music>	mPlayedMusic;
	};
}