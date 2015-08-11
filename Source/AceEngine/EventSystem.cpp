#include "EventSystem.h"

#include <algorithm>

namespace events
{
	std::map<EventType, std::function<void(EventData*)>> EventSystem::mListenerDict;

	//Fire event immediatly
	void EventSystem::fireEvent(EventType type, EventData* eventData)
	{
		if (mListenerDict.find(type) != mListenerDict.end())
		{
			mListenerDict[type](eventData);
		}

		delete eventData;
	}

	//Register listener for Event class
	void EventSystem::registerListener(EventType type, std::function<void(EventData*)> listener)
	{
		mListenerDict[type] = listener;
	}

	//Remove listener
	void EventSystem::removeListener(EventType type)
	{
		mListenerDict.erase(type);
	}
}

