#pragma once
#include <string>
#include <SFML\Audio.hpp>

struct Sound
{
	sf::SoundBuffer	buffer;
	std::string		path;
	bool			isLoaded = false;
};