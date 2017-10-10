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
		AudioSystem(component::EntityWorld& entityWorld);
		~AudioSystem() = default;

		void start();
		void update(float deltaTime);
	private:
		void onPlaySound(events::EventData* eventData);

		utility::ResourceLoader<Sound>				m_soundLoader;
		glm::ivec2									m_movementAxis;
		component::EntityWorld&						m_entityWorld;
		std::forward_list<sf::Sound>				m_playedSounds;
		std::map<component::ElementId, sf::Music>	m_playedMusic;
	};
}