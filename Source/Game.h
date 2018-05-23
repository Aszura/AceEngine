#pragma once
#include "EntityWorld.h"
#include "Texture.h"
#include "Shader.h"
#include "ResourceLoader.h"
#include "SceneLoader.h"
#include "InputCallback.h"

#include "InputSystem.h"
#include "CameraSystem.h"
#include "AudioSystem.h"
#include "RenderSystem.h"
#include "ScriptSystem.h"

#include <memory>


namespace rendering
{
	class Window;
}

namespace component
{
	namespace serialization
	{
		class SceneLoader;
	}
}

namespace game
{
	class Game
	{
	public:
		Game(rendering::Window& windowData, const input::InputCallback& inputCallback);

		void start();
		void run();
		void stop();

	private:
		component::EntityWorld							m_entityWorld;
		utility::ResourceLoader<rendering::Texture>		m_textureLoader;
		utility::ResourceLoader<rendering::Shader>		m_shaderLoader;
		component::serialization::SceneLoader			m_sceneLoader;
		input::InputSystem								m_inputSystem;
		rendering::CameraSystem							m_cameraSystem;
		audio::AudioSystem								m_audioSystem; 
		rendering::RenderSystem							m_renderSystem;
		script::ScriptSystem							m_scriptSystem;
		rendering::Window&								m_windowData;
	};
}
