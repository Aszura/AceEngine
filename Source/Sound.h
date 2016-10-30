#pragma once
#include <string>
#include <SFML\Audio.hpp>

namespace audio
{
	struct Sound
	{
		sf::SoundBuffer	buffer;
		std::string		path;
		bool			isLoaded = false;
	};
}