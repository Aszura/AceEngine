#pragma once
#include "PackedArray.h"
#include "CustomTypes.h"
#include <map>
#include <vector>

namespace component
{
	template<typename T>
	class ComponentWorld
	{
	public:
		ComponentWorld(size_t elementCount)
			: mComponents(elementCount)
		{}

		inline utility::PackedArray<T>& getArray()
		{
			return mComponents;
		}

		inline T* get(ElementId componentId)
		{
			return mComponents.lookup(componentId);
		}

		inline T* getFirst(EntityId id)
		{
			//assert(mEntities.count(id) > 0);
			return mComponents.lookup(mEntities[id][0]);
		}

		inline T* add(EntityId id)
		{
			ElementId elemId = mComponents.add();

			T* component = mComponents.lookup(elemId);
			component->entityId = id;
			component->componentId = elemId;
			mEntities[id].push_back(elemId);

			return component;
		}

		inline void remove(EntityId entityId, ElementId componentId)
		{
			mComponents.remove(componentId);
			mEntities[entityId].erase(componentId);

			if (mEntities.count(entityId) == 0)
			{
				mEntities.erase(entityId);
			}
		}

		inline bool exist(EntityId id)
		{
			return mEntities.count(id) > 0;
		}

	private:
		std::map<EntityId, std::vector<ElementId>> mEntities;
		utility::PackedArray<T> mComponents;
	};
}
