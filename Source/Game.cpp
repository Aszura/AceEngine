#include "Game.h"
#include "InputSystem.h"
#include "CameraSystem.h"
#include "AudioSystem.h"
#include "RenderSystem.h"
#include "SceneLoader.h"
#include "Window.h"

#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>
#include <random>
#include <assert.h>

namespace game
{
	using namespace input;
	using namespace rendering;
	using namespace audio;
	using namespace component::serialization;


	Game::Game(Window& windowData, const InputCallback& inputCallback)
		: m_windowData(windowData)
		, m_sceneLoader(m_entityWorld, m_textureLoader, m_shaderLoader)
		, m_inputSystem(inputCallback)
		, m_cameraSystem(m_entityWorld)
		, m_audioSystem(m_entityWorld)
		, m_renderSystem(m_entityWorld, m_textureLoader, m_shaderLoader, windowData)
	{
	}

	void Game::start()
	{
		bool result = m_sceneLoader.loadScene("Scenes/scene1.xml");
		assert(result);

		m_renderSystem.start(800, 600);
		m_audioSystem.start();
	}


	void Game::run()
	{
		//Calculate delta Time
		static sf::Clock clock;
		sf::Time time = clock.restart();
		float deltaTime = time.asSeconds();

		//Update systems
		m_inputSystem.update();
		m_cameraSystem.update(deltaTime);
		m_audioSystem.update(deltaTime);
		m_renderSystem.update(deltaTime);
	}


	void Game::stop()
	{
	}
}