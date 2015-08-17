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

class EntityWorld;

class AudioSystem
{
public:
	AudioSystem(EntityWorld* entityWorld);
	~AudioSystem();

	void start();
	void update(float deltaTime);
private:
	void onPlaySound(events::EventData* eventData);

	ResourceLoader<Sound>			mSoundLoader;
	glm::ivec2						mMovementAxis;
	EntityWorld*					mEntityWorld;
	std::forward_list<sf::Sound>	mPlayedSounds;
	std::map<ElementId, sf::Music>	mPlayedMusic;
};
