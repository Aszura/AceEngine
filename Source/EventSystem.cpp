#include "EventSystem.h"

#include <algorithm>

namespace events
{
	std::map<EventType, std::function<void(EventData*)>> EventSystem::m_listenerDict;

	//Fire event immediatly
	void EventSystem::fireEvent(EventType type, EventData* eventData)
	{
		if (m_listenerDict.find(type) != m_listenerDict.end())
		{
			m_listenerDict[type](eventData);
		}

		delete eventData;
	}

	//Register listener for Event class
	void EventSystem::registerListener(EventType type, std::function<void(EventData*)> listener)
	{
		m_listenerDict[type] = listener;
	}

	//Remove listener
	void EventSystem::removeListener(EventType type)
	{
		m_listenerDict.erase(type);
	}
}

