#include "InputSystem.h"
#include "EventSystem.h"
#include "EventType.h"
#include "EventData.h"
#include "Window.h"
#include <functional>

namespace input
{
	std::bitset<static_cast<size_t>(KeyMap::size)> InputSystem::m_keys = std::bitset<static_cast<size_t>(KeyMap::size)>();
	std::bitset<static_cast<size_t>(KeyMap::size)> InputSystem::m_lastKeys = std::bitset<static_cast<size_t>(KeyMap::size)>();
	std::bitset<static_cast<size_t>(KeyMap::startKeyboard)> InputSystem::m_mouseButtons = std::bitset<static_cast<size_t>(KeyMap::startKeyboard)>();
	std::bitset<static_cast<size_t>(KeyMap::startKeyboard)> InputSystem::m_lastMouseButtons = std::bitset<static_cast<size_t>(KeyMap::startKeyboard)>();
	glm::ivec2 InputSystem::m_mousePosition = glm::ivec2(0,0);
	glm::ivec2 InputSystem::m_mouseDelta = glm::ivec2(0,0);

	InputSystem::InputSystem(const InputCallback& inputCallback)
		: m_window(nullptr)
		, m_inputCallback(inputCallback)
	{
		using namespace events;
		EventSystem::registerListener(EventType::WindowCreated, [this](EventData* eventData){ onWindowCreated(eventData); });
	}

	void InputSystem::update()
	{
		assert(m_window);

		if (!m_window->hasFocus())
		{
			return;
		}

		glm::ivec2 windowCenter = glm::ivec2(m_window->getSize() / 2u);

		long posX, posY;
		m_inputCallback.mousePosCallback(posX, posY);
		m_mousePosition = m_window->transformPointToScreen( posX, posY);
		m_mouseDelta = m_mousePosition - m_window->transformPointToScreen(windowCenter.x, windowCenter.y);

		if (glm::abs(m_mouseDelta.x) > 0 || glm::abs(m_mouseDelta.y) > 0)
		{
			events::MouseMovedData* eventData = new events::MouseMovedData();
			eventData->mousePosition.x = m_mouseDelta.x;
			eventData->mousePosition.y = m_mouseDelta.y;
			events::EventSystem::fireEvent(events::EventType::MouseMoved, eventData);
		}

		//Reset mouse to center of screen
		//mWindow->setCursorPos(windowCenter.x, windowCenter.y);

		for (unsigned int i = static_cast<int>(KeyMap::startMouse)+1; i < static_cast<int>(KeyMap::startKeyboard); ++i)
		{
			m_lastMouseButtons[i] = m_mouseButtons[i];
			m_mouseButtons[i] = (m_inputCallback.keyCallback(static_cast<int>(i)) > 0) ? 1 : 0;
		}

		for (unsigned int i = static_cast<int>(KeyMap::startKeyboard) + 1; i < static_cast<int>(KeyMap::size); ++i)
		{
			m_lastKeys[i] = m_keys[i];
			m_keys[i] = (m_inputCallback.keyCallback(static_cast<int>(i))) > 0 ? 1 : 0;
		}
	}

	bool InputSystem::isMouseButtonDown(KeyMap button)
	{
		if (m_lastMouseButtons[static_cast<int>(button)] != m_mouseButtons[static_cast<int>(button)])
		{
			return m_mouseButtons[static_cast<int>(button)];
		}

		return false;
	}

	bool InputSystem::isMouseButtonUp(KeyMap button)
	{
		if (m_lastMouseButtons[static_cast<int>(button)] != m_mouseButtons[static_cast<int>(button)])
		{
			return !m_mouseButtons[static_cast<int>(button)];
		}

		return false;
	}

	bool InputSystem::isMouseButtonPressed(KeyMap button)
	{
		return m_mouseButtons[static_cast<int>(button)];
	}

	const glm::ivec2& InputSystem::getMousePosition()
	{
		return m_mousePosition;
	}

	const glm::ivec2& InputSystem::getMouseDelta()
	{
		return m_mouseDelta;
	}

	bool InputSystem::isKeyDown(KeyMap key)
	{
		if (m_lastKeys[static_cast<int>(key)] != m_keys[static_cast<int>(key)])
		{
			return m_keys[static_cast<int>(key)];
		}

		return false;
	}

	bool InputSystem::isKeyUp(KeyMap key)
	{
		if (m_lastKeys[static_cast<int>(key)] != m_keys[static_cast<int>(key)])
		{
			return !m_keys[static_cast<int>(key)];
		}

		return false;
	}

	bool InputSystem::isKeyPressed(KeyMap key)
	{
		return m_keys[static_cast<int>(key)];
	}

	void InputSystem::onWindowCreated(events::EventData* eventData)
	{
		using namespace events;

		if (WindowCreatedData* data = static_cast<WindowCreatedData*>(eventData))
		{
			m_window = data->window;
			glm::ivec2 windowCenter = glm::ivec2(m_window->getSize() / 2u);
			windowCenter = m_window->transformPointToScreen(windowCenter.x, windowCenter.y);
			m_window->setCursorPos(windowCenter.x, windowCenter.y);
		}
	}
}