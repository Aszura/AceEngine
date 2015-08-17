#pragma once
#include "EntityWorld.h"
#include "Texture.h"
#include "Shader.h"
#include "ResourceLoader.h"

class SceneLoader;
class TerrainSystem;
class InputSystem;
class PlayerSystem;
class CameraSystem;
class PhysicsSystem;
class RenderSystem;
class AudioSystem;

class Game
{
public:
	Game();
	~Game();

	void start();
	void run();
	void stop();

private:
	EntityWorld						mEntityWorld;
	ResourceLoader<Texture>			mTextureLoader;
	ResourceLoader<Shader>			mShaderLoader;
	SceneLoader*						mSceneLoader;
	TerrainSystem*					mTerrainSystem;
	InputSystem*					mInputSystem;
	//PlayerSystem*					mPlayerSystem;
	CameraSystem*					mCameraSystem;
	PhysicsSystem*					mPhysicsSystem;
	AudioSystem*					mAudioSystem;
	RenderSystem*					mRenderSystem;
};

