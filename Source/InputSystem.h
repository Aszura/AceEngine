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
		~InputSystem() = default;

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
		static std::bitset<static_cast<size_t>(KeyMap::size)> m_keys;
		static std::bitset<static_cast<size_t>(KeyMap::size)> m_lastKeys;
		static std::bitset<static_cast<size_t>(KeyMap::startKeyboard)> m_mouseButtons;
		static std::bitset<static_cast<size_t>(KeyMap::startKeyboard)> m_lastMouseButtons;
		static glm::ivec2 m_mousePosition;
		static glm::ivec2 m_mouseDelta;
		rendering::Window* m_window;
		const InputCallback& m_inputCallback;
	};
}