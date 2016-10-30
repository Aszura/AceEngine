#pragma once
#include "CustomTypes.h"

namespace physx
{
	class PxMaterial;
	class PxRigidStatic;
	class PxShape;
};

namespace component
{
	struct CapsuleColliderComponent
	{
		EntityId				entityId;
		ElementId				componentId;
		physx::PxRigidStatic*	staticBody = nullptr;
		physx::PxShape*			shape = nullptr;
		physx::PxMaterial*		material = nullptr;
		float					radius = 0.5f;
		float					height = 1.0f;
		bool					isDynamic = false;
	};
}