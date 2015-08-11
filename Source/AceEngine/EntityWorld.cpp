#include "EntityWorld.h"

EntityId EntityWorld::getUnusedEntityId()
{
	return m_freeIdCounter++;
}