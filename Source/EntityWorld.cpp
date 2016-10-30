#include "EntityWorld.h"

namespace component
{
	EntityId EntityWorld::getUnusedEntityId()
	{
		return m_freeIdCounter++;
	}
}