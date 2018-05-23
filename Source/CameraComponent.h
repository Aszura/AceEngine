#pragma once
#include "CustomTypes.h"
#include <glm\glm.hpp>

namespace component
{
	struct CameraComponent
	{
		EntityId	entityId;
		ElementId	componentId;
		glm::vec3	direction = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3	forward = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3	right = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3	up = glm::vec3(0.0, 1.0f, 0.0f);
		glm::vec2	viewportSize;
		float		pitch = 0.0f;
		float		yaw = 0.0f;
		float		roll = 0.0f;
		float		fov = 65.0f;
		float		zNear = -10.0f;
		float		zFar = 10.0f;
	};
}