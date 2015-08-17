#pragma once
#include "CustomTypes.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>

struct TransformComponent
{
	EntityId	entityId;
	ElementId	componentId;
	glm::vec3	position = glm::vec3(0.0f);
	glm::vec3	scale = glm::vec3(1.0f);
	glm::quat	rotation = glm::quat();
};