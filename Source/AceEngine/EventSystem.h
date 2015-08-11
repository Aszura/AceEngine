#pragma once

#include "EventType.h"
#include "EventData.h"

#include <functional>
#include <map>
#include <vector>

namespace events
{
	class EventSystem
	{
	public:
		static void fireEvent(EventType type, EventData* data);
		static void registerListener(EventType type, std::function<void(EventData*)> callback);
		static void removeListener(EventType type);

	private:
		static std::map<EventType, std::function<void(EventData*)>> mListenerDict;
	};
}
