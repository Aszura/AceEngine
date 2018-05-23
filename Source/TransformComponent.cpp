#include "TransformComponent.h"

float getPositionX(uint64_t objPtr)
{
	auto comp = reinterpret_cast<component::TransformComponent*>(objPtr);
	return comp->position.x;
}

float getPositionY(uint64_t objPtr)
{
	auto comp = reinterpret_cast<component::TransformComponent*>(objPtr);
	return comp->position.y;
}

float getPositionZ(uint64_t objPtr)
{
	auto comp = reinterpret_cast<component::TransformComponent*>(objPtr);
	return comp->position.z;
}

void setPosition(uint64_t objPtr, float* value)
{
	auto comp = reinterpret_cast<component::TransformComponent*>(objPtr);
	comp->position[0] = value[0];
	comp->position[1] = value[1];
	comp->position[2] = value[2];
}
