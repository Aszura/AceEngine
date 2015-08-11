#pragma once
#include "CustomTypes.h"
#include "Texture.h"
#include <string>

struct SkyboxComponent
{
	Texture*		textures[6];
	EntityId		entityId;
	ElementId		componentId;
	unsigned int	cubemapId;
};