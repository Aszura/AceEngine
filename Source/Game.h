#pragma once
#include "EntityWorld.h"
#include "Texture.h"
#include "Shader.h"
#include "ResourceLoader.h"
#include "InputCallback.h"


namespace input
{
	class InputSystem;
}

namespace physics
{
	class PhysicsSystem;
}

namespace rendering
{
	class CameraSystem;
	class RenderSystem;
	class Window;
}

namespace audio
{
	class AudioSystem;
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
		Game(rendering::Window* windowData, const input::InputCallback& inputCallback);
		~Game();

		void start();
		void run();
		void stop();

	private:
		component::EntityWorld							mEntityWorld;
		utility::ResourceLoader<rendering::Texture>		mTextureLoader;
		utility::ResourceLoader<rendering::Shader>		mShaderLoader;
		component::serialization::SceneLoader*			mSceneLoader;
		input::InputSystem*								mInputSystem;
		rendering::CameraSystem*						mCameraSystem;
		physics::PhysicsSystem*							mPhysicsSystem;
		audio::AudioSystem*								mAudioSystem;
		rendering::RenderSystem*						mRenderSystem;

		rendering::Window*								mWindowData;
	};
}
