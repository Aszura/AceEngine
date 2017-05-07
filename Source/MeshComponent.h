#pragma once
#include "CustomTypes.h"
#include "SpriteData.h"
#include "Material.h"
#include <glm\glm.hpp>
#include <vector>

namespace component
{
	struct MeshComponent
	{
		rendering::SpriteData		spriteData;
		std::vector<glm::uvec3>		indices;
		rendering::Material*		material;
		EntityId					entityId;
		ElementId					componentId;
	};
}