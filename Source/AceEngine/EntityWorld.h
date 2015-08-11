#pragma once
#include "CameraComponent.h"
#include "MeshComponent.h"
#include "TransformComponent.h"
#include "LightComponent.h"
#include "TerrainComponent.h"
#include "PlayerComponent.h"
#include "RigidbodyComponent.h"
#include "CharacterControllerComponent.h"
#include "CapsuleColliderComponent.h"
#include "MusicComponent.h"
#include "SkyboxComponent.h"
#include "ComponentWorld.h"
#include "CustomTypes.h"

//Macro for declaring the map of a component plus getter
#define DECLARE_COMPONENT_TYPE(ComponentType, ComponentName, Length) public: \
public: \
	inline ComponentWorld<ComponentType>& get##ComponentName##World() \
	{ \
		return m##ComponentName##World; \
	} \
private: \
	ComponentWorld<ComponentType> m##ComponentName##World = ComponentWorld<ComponentType>(Length);

class EntityWorld
{
	//Components
	DECLARE_COMPONENT_TYPE(MeshComponent, Mesh, 4096);
	DECLARE_COMPONENT_TYPE(TransformComponent, Transform, 4096);
	DECLARE_COMPONENT_TYPE(RigidbodyComponent, Rigidbody, 4096);
	DECLARE_COMPONENT_TYPE(CharacterControllerComponent, CharacterController, 4096);
	DECLARE_COMPONENT_TYPE(CapsuleColliderComponent, CapsuleCollider, 4096);
	DECLARE_COMPONENT_TYPE(LightComponent, Light, 128);
	DECLARE_COMPONENT_TYPE(MusicComponent, Music, 16);
	DECLARE_COMPONENT_TYPE(CameraComponent, Camera, 16);
	DECLARE_COMPONENT_TYPE(TerrainComponent, Terrain, 16);
	DECLARE_COMPONENT_TYPE(PlayerComponent, Player, 16);
	DECLARE_COMPONENT_TYPE(SkyboxComponent, Skybox, 16);

public:
	EntityId getUnusedEntityId();

private:
	EntityId m_freeIdCounter = 1;

};