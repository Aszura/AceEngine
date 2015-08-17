#include "InputSystem.h"
#include "EventSystem.h"
#include "EventType.h"
#include "EventData.h"
#include <SFML\Window.hpp>
#include <functional>

std::bitset<static_cast<size_t>(InputSystem::Key::KeyCount)> InputSystem::mKeys = std::bitset<static_cast<size_t>(InputSystem::Key::KeyCount)>();
std::bitset<static_cast<size_t>(InputSystem::Key::KeyCount)> InputSystem::mLastKeys = std::bitset<static_cast<size_t>(InputSystem::Key::KeyCount)>();
std::bitset<static_cast<size_t>(InputSystem::MouseButton::ButtonCount)> InputSystem::mMouseButtons = std::bitset<static_cast<size_t>(InputSystem::MouseButton::ButtonCount)>();
std::bitset<static_cast<size_t>(InputSystem::MouseButton::ButtonCount)> InputSystem::mLastMouseButtons = std::bitset<static_cast<size_t>(InputSystem::MouseButton::ButtonCount)>();
glm::ivec2 InputSystem::mMousePosition = glm::ivec2(0,0);
glm::ivec2 InputSystem::mMouseDelta = glm::ivec2(0,0);

InputSystem::InputSystem()
	: mWindow(nullptr)
	//, mLastMousePosition(glm::ivec2(0))
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

	sf::Vector2i windowCenter = sf::Vector2i(mWindow->getSize() / 2u);

	sf::Vector2i sfMousePos = sf::Mouse::getPosition(*mWindow);
	mMousePosition = glm::ivec2(sfMousePos.x, sfMousePos.y);
	mMouseDelta = mMousePosition - glm::ivec2(windowCenter.x, windowCenter.y);
	//mLastMousePosition = mMousePosition;

	if (glm::abs(mMouseDelta.x) > 0 || glm::abs(mMouseDelta.y) > 0)
	{
		events::MouseMovedData* eventData = new events::MouseMovedData();
		eventData->mousePosition.x = mMouseDelta.x;
		eventData->mousePosition.y = mMouseDelta.y;
		events::EventSystem::fireEvent(events::EventType::MouseMoved, eventData);
	}

	//Reset mouse to center of screen
	sf::Mouse::setPosition(windowCenter, *mWindow);

	for (unsigned int i = 0; i < sf::Mouse::ButtonCount; ++i)
	{
		mLastMouseButtons[i] = mMouseButtons[i];
		mMouseButtons[i] = sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));
	}

	for (unsigned int i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		mLastKeys[i] = mKeys[i];
		mKeys[i] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));
	}
}

bool InputSystem::isMouseButtonDown(const MouseButton& button)
{
	if (mLastMouseButtons[static_cast<int>(button)] != mMouseButtons[static_cast<int>(button)])
	{
		return mMouseButtons[static_cast<int>(button)];
	}

	return false;
}

bool InputSystem::isMouseButtonUp(const MouseButton& button)
{
	if (mLastMouseButtons[static_cast<int>(button)] != mMouseButtons[static_cast<int>(button)])
	{
		return !mMouseButtons[static_cast<int>(button)];
	}

	return false;
}

bool InputSystem::isMouseButtonPressed(const MouseButton& button)
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

bool InputSystem::isKeyDown(const Key& key)
{
	if (mLastKeys[static_cast<int>(key)] != mKeys[static_cast<int>(key)])
	{
		return mKeys[static_cast<int>(key)];
	}

	return false;
}

bool InputSystem::isKeyUp(const Key& key)
{
	if (mLastKeys[static_cast<int>(key)] != mKeys[static_cast<int>(key)])
	{
		return !mKeys[static_cast<int>(key)];
	}

	return false;
}

bool InputSystem::isKeyPressed(const Key& key)
{
	return mKeys[static_cast<int>(key)];
}

void InputSystem::onWindowCreated(events::EventData* eventData)
{
	using namespace events;

	if (WindowCreatedData* data = static_cast<WindowCreatedData*>(eventData))
	{
		mWindow = data->window;
		sf::Vector2i windowCenter = sf::Vector2i(mWindow->getSize() / 2u);
		sf::Mouse::setPosition(windowCenter, *mWindow);
	}
}