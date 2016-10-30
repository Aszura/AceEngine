#pragma once
#include "CustomTypes.h"
#include "Texture.h"
#include <string>

namespace component
{
	struct SkyboxComponent
	{
		rendering::Texture*		textures[6];
		component::EntityId		entityId;
		component::ElementId	componentId;
		unsigned int			cubemapId;
	};
}