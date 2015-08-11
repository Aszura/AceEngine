#pragma once
#include "CustomTypes.h"
#include <glm\glm.hpp>

struct LightComponent
{
	EntityId	entityId;
	ElementId	componentId;
	glm::vec4	color = glm::vec4(1.0f);
	float		intensity = 1.0f;
};