#include "Game.h"
#include "TerrainSystem.h"
#include "InputSystem.h"
//#include "PlayerSystem.h"
#include "CameraSystem.h"
//#include "GuiSystem.h"
//#include "AnimationSystem.h"
#include "PhysicsSystem.h"
#include "AudioSystem.h"
#include "RenderSystem.h"
#include "SceneLoader.h"

#include <SFML\System.hpp>
#include <SFML\Graphics.hpp>
#include <random>
#include <assert.h>

Game::Game()
{
	mSceneLoader = new SceneLoader(&mEntityWorld, &mTextureLoader, &mShaderLoader);
	mTerrainSystem = new TerrainSystem(&mEntityWorld);
	mInputSystem = new InputSystem();
	//mPlayerSystem = new PlayerSystem(&mEntityWorld);
	mCameraSystem = new CameraSystem(&mEntityWorld);
	//mGuiSystem = new GuiSystem(&mEntityWorld);
	//mAnimationSystem = new AnimationSystem(&mEntityWorld);
	mPhysicsSystem = new PhysicsSystem(&mEntityWorld);
	mAudioSystem = new AudioSystem(&mEntityWorld);
	mRenderSystem = new RenderSystem(&mEntityWorld, &mTextureLoader, &mShaderLoader);
}


Game::~Game()
{
	delete mSceneLoader;
	delete mTerrainSystem;
	delete mInputSystem;
	//delete mPlayerSystem;
	delete mCameraSystem;
	//delete mGuiSystem;
	//delete mAnimationSystem;
	delete mPhysicsSystem;
	delete mAudioSystem;
	delete mRenderSystem;
}

void Game::start()
{
	mSceneLoader->loadScene("../../Scenes/scene1.xml");

	////Create player & camera
	//{
	//	EntityId entityId = mEntityWorld.getUnusedEntityId();
	//	mEntityWorld.getPlayerWorld().add(entityId);
	//	mEntityWorld.getCameraWorld().add(entityId);
	//	mEntityWorld.getCharacterControllerWorld().add(entityId);
	//	mEntityWorld.getTransformWorld().add(entityId);
	//}

	////Create skybox
	//{
	//	EntityId entityId = mEntityWorld.getUnusedEntityId();
	//	SkyboxComponent* skyboxComp = mEntityWorld.getSkyboxWorld().add(entityId);
	//	assert(skyboxComp); 
	//	skyboxComp->textures[0] = mTextureLoader.load("../../Media/Textures/Skybox/clouds1_east.bmp");
	//	skyboxComp->textures[1] = mTextureLoader.load("../../Media/Textures/Skybox/clouds1_west.bmp");
	//	skyboxComp->textures[2] = mTextureLoader.load("../../Media/Textures/Skybox/clouds1_up.bmp");
	//	skyboxComp->textures[3] = mTextureLoader.load("../../Media/Textures/Skybox/clouds1_down.bmp");
	//	skyboxComp->textures[4] = mTextureLoader.load("../../Media/Textures/Skybox/clouds1_north.bmp");
	//	skyboxComp->textures[5] = mTextureLoader.load("../../Media/Textures/Skybox/clouds1_south.bmp");
	//	skyboxComp->textures[0]->type = TextureType::CubemapPosX;
	//	skyboxComp->textures[1]->type = TextureType::CubemapNegX;
	//	skyboxComp->textures[2]->type = TextureType::CubemapPosY;
	//	skyboxComp->textures[3]->type = TextureType::CubemapNegY;
	//	skyboxComp->textures[4]->type = TextureType::CubemapPosZ;
	//	skyboxComp->textures[5]->type = TextureType::CubemapNegZ;
	//}

	////Create Light
	//{
	//	EntityId entityId = mEntityWorld.getUnusedEntityId();
	//	TransformComponent* transformComp = mEntityWorld.getTransformWorld().add(entityId);
	//	assert(transformComp);
	//	transformComp->position = glm::vec3(0.25f, 0.25f, 0.5f);

	//	LightComponent* lightComp = mEntityWorld.getLightWorld().add(entityId);
	//	assert(lightComp);
	//	lightComp->intensity = 2.0f;
	//}

	////Create Terrain
	//{
	//	EntityId entityId = mEntityWorld.getUnusedEntityId();
	//	TransformComponent* transformComp = mEntityWorld.getTransformWorld().add(entityId);
	//	assert(transformComp);
	//	transformComp->position = glm::vec3(-256.0f, -100.0f, -256.0f);

	//	TerrainComponent* terrainComp = mEntityWorld.getTerrainWorld().add(entityId);
	//	assert(terrainComp);
	//	terrainComp->terrainScale = 2.0f;
	//	terrainComp->material = new Material();
	//	terrainComp->material->shader = mShaderLoader.load("Terrain");
	//	terrainComp->material->ambient = glm::vec4(0.25f, 0.25f, 0.25f, 0.0f);
	//	terrainComp->material->texture = mTextureLoader.load("../../Media/Textures/grass_green_d.jpg");
	//	terrainComp->heightTexture = mTextureLoader.load("../../Media/Textures/terrain1.png");
	//}

	////Create ball
	//{
	//	EntityId entityId = mEntityWorld.getUnusedEntityId();
	//	TransformComponent* transformComp = mEntityWorld.getTransformWorld().add(entityId);
	//	assert(transformComp);
	//	transformComp->position = glm::vec3(0.0f, -2.0f, -10.0f);
	//	mSceneImporter->loadScene(entityId, "../../Media/Models/bouncingball1.dae", "Diffuse");

	//	MeshComponent* meshComp = mEntityWorld.getMeshWorld().getFirst(entityId);
	//	assert(meshComp);
	//	meshComp->material->texture = mTextureLoader.load("../../Media/Textures/grass_green_d.jpg");
	//}

	////Create music
	//{
	//	EntityId entityId = mEntityWorld.getUnusedEntityId();
	//	MusicComponent* musicComp = mEntityWorld.getMusicWorld().add(entityId);
	//	assert(musicComp);
	//	musicComp->filePath = "../../Media/Audio/wind.ogg";
	//	musicComp->isPlaying = true;
	//}

	mTerrainSystem->start();
	mRenderSystem->start(800,600);
	mPhysicsSystem->start();
	mAudioSystem->start();
}


void Game::run()
{
	sf::Clock clock;
	bool running = true;

	do
	{
		//Calculate delta Time
		sf::Time time = clock.restart();
		float deltaTime = time.asSeconds();

		//Update systems
		mInputSystem->update();
		//mPlayerSystem->update(deltaTime);
		//mGuiSystem->update();
		//mAnimationSystem->update(deltaTime);
		mPhysicsSystem->update(deltaTime);
		mCameraSystem->update(deltaTime);
		mAudioSystem->update(deltaTime);
		mRenderSystem->update(deltaTime, running);

		running = !InputSystem::isKeyUp(InputSystem::Key::Escape);
	} while (running);
}


void Game::stop()
{

}