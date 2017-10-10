#pragma once
#include "CameraComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "CharacterControllerComponent.h"
#include "CapsuleColliderComponent.h"
#include "MusicComponent.h"
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

namespace component
{
	class EntityWorld
	{
		//Components
		DECLARE_COMPONENT_TYPE(SpriteComponent, Sprite, 4096);
		DECLARE_COMPONENT_TYPE(TransformComponent, Transform, 4096);
		DECLARE_COMPONENT_TYPE(CharacterControllerComponent, CharacterController, 4096);
		DECLARE_COMPONENT_TYPE(CapsuleColliderComponent, CapsuleCollider, 4096);
		DECLARE_COMPONENT_TYPE(MusicComponent, Music, 16);
		DECLARE_COMPONENT_TYPE(CameraComponent, Camera, 16);

	public:
		EntityId getUnusedEntityId();

	private:
		EntityId m_freeIdCounter = 1;

	};
}