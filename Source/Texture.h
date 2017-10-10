#pragma once
#include <string>
#include <glm\glm.hpp>
#include <SFML\Graphics.hpp>

namespace rendering
{
	struct Texture
	{
		enum class FilteringType : unsigned char
		{
			None = 0,
			Bilinear,
			Trilinear
		};

		enum class CompressionType : unsigned char
		{
			None,
			DXT5,
		};

		sf::Image		image;
		std::string		path;
		glm::uvec2		size;
		unsigned int	id;
		bool			isLoaded = false;
		bool			sRGB = true;
		FilteringType   filteringLevel = FilteringType::None;
		CompressionType compressionLevel = CompressionType::None;
		unsigned char   anisotropicLevel = 1;
	};
}