#pragma once
#include <string>
#include <glm\glm.hpp>
#include <SFML\Graphics.hpp>

enum class TextureType
{
	Texture2D = 0,
	CubemapPosX,
	CubemapNegX,
	CubemapPosY,
	CubemapNegY,
	CubemapPosZ,
	CubemapNegZ,
};

struct Texture
{
	sf::Image		image;
	std::string		path;
	glm::uvec2		size;
	unsigned int	id;
	TextureType		type = TextureType::Texture2D;
	bool			isLoaded = false;
	bool			sRGB = true;
};