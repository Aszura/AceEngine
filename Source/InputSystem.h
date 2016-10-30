#pragma once
#include <bitset>
#include <glm\glm.hpp>

#include "InputCallback.h"
#include "KeyMap.h"

namespace rendering
{
	class Window;
}
class ComponentEntityWorld;

namespace events
{
	struct EventData;
}

namespace input
{
	class InputSystem
	{
	public:
		InputSystem(const InputCallback& inputCallback);
		void update();

		static bool isMouseButtonDown(KeyMap button);
		static bool isMouseButtonUp(KeyMap button);
		static bool isMouseButtonPressed(KeyMap button);
		static const glm::ivec2& getMousePosition();
		static const glm::ivec2& getMouseDelta();

		static bool isKeyDown(KeyMap key);
		static bool isKeyUp(KeyMap key);
		static bool isKeyPressed(KeyMap key);

		void onWindowCreated(events::EventData* eventData);
	private:
		static std::bitset<static_cast<size_t>(KeyMap::size)> mKeys;
		static std::bitset<static_cast<size_t>(KeyMap::size)> mLastKeys;
		static std::bitset<static_cast<size_t>(KeyMap::startKeyboard)> mMouseButtons;
		static std::bitset<static_cast<size_t>(KeyMap::startKeyboard)> mLastMouseButtons;
		static glm::ivec2 mMousePosition;
		static glm::ivec2 mMouseDelta;
		rendering::Window* mWindow;
		const InputCallback& mInputCallback;
	};
}