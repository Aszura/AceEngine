#pragma once
#include "CustomTypes.h"

namespace physx
{
	class PxRigidDynamic;
};

namespace component
{
	struct RigidbodyComponent
	{
		EntityId				entityId;
		ElementId				componentId;
		physx::PxRigidDynamic*	rigidbody = nullptr;
	};
}