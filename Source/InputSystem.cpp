#include "InputSystem.h"
#include "EventSystem.h"
#include "EventType.h"
#include "EventData.h"
#include "Window.h"
#include <functional>

namespace input
{
	std::bitset<static_cast<size_t>(KeyMap::size)> InputSystem::mKeys = std::bitset<static_cast<size_t>(KeyMap::size)>();
	std::bitset<static_cast<size_t>(KeyMap::size)> InputSystem::mLastKeys = std::bitset<static_cast<size_t>(KeyMap::size)>();
	std::bitset<static_cast<size_t>(KeyMap::startKeyboard)> InputSystem::mMouseButtons = std::bitset<static_cast<size_t>(KeyMap::startKeyboard)>();
	std::bitset<static_cast<size_t>(KeyMap::startKeyboard)> InputSystem::mLastMouseButtons = std::bitset<static_cast<size_t>(KeyMap::startKeyboard)>();
	glm::ivec2 InputSystem::mMousePosition = glm::ivec2(0,0);
	glm::ivec2 InputSystem::mMouseDelta = glm::ivec2(0,0);

	InputSystem::InputSystem(const InputCallback& inputCallback)
		: mWindow(nullptr)
		, mInputCallback(inputCallback)
	{
		using namespace events;
		EventSystem::registerListener(EventType::WindowCreated, [this](EventData* eventData){ onWindowCreated(eventData); });
	}

	void InputSystem::update()
	{
		assert(mWindow);

		if (!mWindow->hasFocus())
		{
			return;
		}

		glm::ivec2 windowCenter = glm::ivec2(mWindow->getSize() / 2u);

		long posX, posY;
		mInputCallback.mousePosCallback(posX, posY);
		mMousePosition = mWindow->transformPointToScreen( posX, posY);
		mMouseDelta = mMousePosition - mWindow->transformPointToScreen(windowCenter.x, windowCenter.y);

		if (glm::abs(mMouseDelta.x) > 0 || glm::abs(mMouseDelta.y) > 0)
		{
			events::MouseMovedData* eventData = new events::MouseMovedData();
			eventData->mousePosition.x = mMouseDelta.x;
			eventData->mousePosition.y = mMouseDelta.y;
			events::EventSystem::fireEvent(events::EventType::MouseMoved, eventData);
		}

		//Reset mouse to center of screen
		mWindow->setCursorPos(windowCenter.x, windowCenter.y);

		for (unsigned int i = static_cast<int>(KeyMap::startMouse)+1; i < static_cast<int>(KeyMap::startKeyboard); ++i)
		{
			mLastMouseButtons[i] = mMouseButtons[i];
			mMouseButtons[i] = static_cast<bool>(mInputCallback.keyCallback(static_cast<int>(i)));
		}

		for (unsigned int i = static_cast<int>(KeyMap::startKeyboard) + 1; i < static_cast<int>(KeyMap::size); ++i)
		{
			mLastKeys[i] = mKeys[i];
			mKeys[i] = static_cast<bool>(mInputCallback.keyCallback(static_cast<int>(i)));
		}
	}

	bool InputSystem::isMouseButtonDown(KeyMap button)
	{
		if (mLastMouseButtons[static_cast<int>(button)] != mMouseButtons[static_cast<int>(button)])
		{
			return mMouseButtons[static_cast<int>(button)];
		}

		return false;
	}

	bool InputSystem::isMouseButtonUp(KeyMap button)
	{
		if (mLastMouseButtons[static_cast<int>(button)] != mMouseButtons[static_cast<int>(button)])
		{
			return !mMouseButtons[static_cast<int>(button)];
		}

		return false;
	}

	bool InputSystem::isMouseButtonPressed(KeyMap button)
	{
		return mMouseButtons[static_cast<int>(button)];
	}

	const glm::ivec2& InputSystem::getMousePosition()
	{
		return mMousePosition;
	}

	const glm::ivec2& InputSystem::getMouseDelta()
	{
		return mMouseDelta;
	}

	bool InputSystem::isKeyDown(KeyMap key)
	{
		if (mLastKeys[static_cast<int>(key)] != mKeys[static_cast<int>(key)])
		{
			return mKeys[static_cast<int>(key)];
		}

		return false;
	}

	bool InputSystem::isKeyUp(KeyMap key)
	{
		if (mLastKeys[static_cast<int>(key)] != mKeys[static_cast<int>(key)])
		{
			return !mKeys[static_cast<int>(key)];
		}

		return false;
	}

	bool InputSystem::isKeyPressed(KeyMap key)
	{
		return mKeys[static_cast<int>(key)];
	}

	void InputSystem::onWindowCreated(events::EventData* eventData)
	{
		using namespace events;

		if (WindowCreatedData* data = static_cast<WindowCreatedData*>(eventData))
		{
			mWindow = data->window;
			glm::ivec2 windowCenter = glm::ivec2(mWindow->getSize() / 2u);
			windowCenter = mWindow->transformPointToScreen(windowCenter.x, windowCenter.y);
			mWindow->setCursorPos(windowCenter.x, windowCenter.y);
		}
	}
}