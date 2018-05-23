#pragma once
#include "CustomTypes.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>

namespace component
{
	struct TransformComponent
	{
		EntityId	entityId;
		ElementId	componentId;
		glm::vec3	position = glm::vec3(0.0f);
		glm::vec3	scale = glm::vec3(1.0f);
		glm::quat	rotation = glm::quat();
	};
}

extern "C" __declspec(dllexport) float getPositionX(uint64_t objPtr);
extern "C" __declspec(dllexport) float getPositionY(uint64_t objPtr);
extern "C" __declspec(dllexport) float getPositionZ(uint64_t objPtr);
extern "C" __declspec(dllexport) void setPosition(uint64_t objPtr, float* value);