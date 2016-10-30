#pragma once
#include "CustomTypes.h"
#include <string>

namespace component
{
	struct MusicComponent
	{
		std::string	filePath;
		EntityId	entityId;
		ElementId	componentId;
		bool		isPlaying = false;
		bool		isLooping = true;
	};
}