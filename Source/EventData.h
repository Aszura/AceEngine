#pragma once
#include "CustomTypes.h"

#include <glm\glm.hpp>
#include <string>

namespace sf
{
	class Window;
}

namespace events
{
	struct EventData
	{};

	struct MouseMovedData : public EventData
	{
		glm::ivec2 mousePosition;
	};

	struct PlaySoundData : public EventData
	{
		std::string path;
	};

	struct WindowCreatedData : public EventData
	{
		sf::Window* window;
	};
}
