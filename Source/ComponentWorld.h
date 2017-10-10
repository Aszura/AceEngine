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
			: m_components(elementCount)
		{}

		inline utility::PackedArray<T>& getArray()
		{
			return m_components;
		}

		inline T* get(ElementId componentId)
		{
			return m_components.lookup(componentId);
		}

		inline T* getFirst(EntityId id)
		{
			//assert(mEntities.count(id) > 0);

			if (exist(id))
			{
				return m_components.lookup(m_entities[id][0]);
			}
			else
			{
				return nullptr;
			}
		}

		inline T* add(EntityId id)
		{
			ElementId elemId = m_components.add();

			T* component = m_components.lookup(elemId);
			component->entityId = id;
			component->componentId = elemId;
			m_entities[id].push_back(elemId);

			return component;
		}

		inline void remove(EntityId entityId, ElementId componentId)
		{
			m_components.remove(componentId);
			m_entities[entityId].erase(componentId);

			if (m_entities.count(entityId) == 0)
			{
				m_entities.erase(entityId);
			}
		}

		inline bool exist(EntityId id)
		{
			return m_entities.count(id) > 0;
		}

	private:
		std::map<EntityId, std::vector<ElementId>> m_entities;
		utility::PackedArray<T> m_components;
	};
}
