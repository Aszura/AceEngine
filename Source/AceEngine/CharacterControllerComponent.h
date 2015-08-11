#pragma once
#include "CustomTypes.h"

#include <glm\glm.hpp>

namespace physx
{
	class PxController;
};

struct CharacterControllerComponent
{
	glm::vec3				force = glm::vec3(0.0f);
	glm::vec3				velocity = glm::vec3(0.0f);
	EntityId				entityId;
	ElementId				componentId;
	physx::PxController*	controller = nullptr;
	float					radius = 0.5f;
	float					height = 1.8f;
	float					slopeLimit = 0.5f;
	bool					isOnGround = false;
};