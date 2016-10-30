#pragma once
#include "CustomTypes.h"
#include <glm\glm.hpp>

namespace component
{
	struct LightComponent
	{
		EntityId	entityId;
		ElementId	componentId;
		glm::vec4	color = glm::vec4(1.0f);
		glm::vec4	ambient = glm::vec4(0.05f);
		float		intensity = 1.0f;
	};
}