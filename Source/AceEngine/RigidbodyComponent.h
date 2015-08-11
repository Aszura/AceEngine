#pragma once
#include "CustomTypes.h"

namespace physx
{
	class PxRigidDynamic;
};

struct RigidbodyComponent
{
	EntityId				entityId;
	ElementId				componentId;
	physx::PxRigidDynamic*	rigidbody = nullptr;
};