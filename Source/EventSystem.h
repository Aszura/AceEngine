#pragma once

#include "EventData.h"
#include "ApiExports.h"

#include <functional>
#include <map>
#include <vector>

namespace events
{
	class EventSystem
	{
	public:
		static ACE_ENGINE_API void fireEvent(const std::string& type, EventData* data);
		static ACE_ENGINE_API void registerListener(const std::string& type, std::function<void(EventData*)> callback);
		static ACE_ENGINE_API void removeListener(const std::string& type);

	private:
		static std::map<std::string, std::function<void(EventData*)>> m_listenerDict;
	};
}
