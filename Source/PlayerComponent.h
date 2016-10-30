#pragma once
#include "CustomTypes.h"
#include <glm\glm.hpp>

namespace component
{
	struct PlayerComponent
	{
		EntityId	entityId;
		ElementId	componentId;
	};
}