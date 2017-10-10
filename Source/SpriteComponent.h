#pragma once
#include "CustomTypes.h"
#include "SpriteData.h"
#include "Material.h"
#include <glm\glm.hpp>
#include <vector>

namespace component
{
	struct SpriteComponent
	{
		std::string shaderPath;
		std::string texturePath;
		rendering::SpriteData		spriteData;
		std::vector<glm::uvec3>		indices;
		rendering::Material*		material;
		EntityId					entityId;
		ElementId					componentId;
	};
}