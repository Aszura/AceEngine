#pragma once
#include <string>
#include <glm\glm.hpp>
#include <SFML\Graphics.hpp>

namespace rendering
{
	struct Texture
	{
		sf::Image		image;
		std::string		path;
		glm::uvec2		size;
		unsigned int	id;
		bool			isLoaded = false;
		bool			sRGB = true;
	};
}