#pragma once
#include "CustomTypes.h"
#include <string>

struct MusicComponent
{
	std::string	filePath;
	EntityId	entityId;
	ElementId	componentId;
	bool		isPlaying = false; 
	bool		isLooping = true;
};