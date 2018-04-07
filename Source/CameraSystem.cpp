#include "CameraSystem.h"
#include "InputSystem.h"
#include "EntityWorld.h"
#include "EventSystem.h"

#include <glm\ext.hpp>

namespace rendering
{
	CameraSystem::CameraSystem(component::EntityWorld& entityWorld)
		: m_entityWorld(entityWorld)
	{
	}

	void CameraSystem::start()
	{
	}

	void CameraSystem::update(float deltaTime)
	{
		using namespace component;
		using namespace input;

		const float playerSpeed = 10.0f;
		const float lookSensitivity = 10.30f;
		const float jumpForce = 35.0f;


		if (m_entityWorld.getCameraWorld().getArray().length() > 0)
		{
			CameraComponent& cameraComp = *m_entityWorld.getCameraWorld().getArray().begin();
			TransformComponent* cameraTransform = m_entityWorld.getTransformWorld().getFirst(cameraComp.entityId);
			assert(cameraTransform);

			if (InputSystem::isKeyDown(KeyMap::vk_Up) || InputSystem::isKeyDown(KeyMap::vk_W))
			{
				m_movementAxis.y -= 1;
			}
			if (InputSystem::isKeyUp(KeyMap::vk_Up) || InputSystem::isKeyUp(KeyMap::vk_W))
			{
				m_movementAxis.y += 1;
			}

			if (InputSystem::isKeyDown(KeyMap::vk_Left) || InputSystem::isKeyDown(KeyMap::vk_A))
			{
				m_movementAxis.x -= 1;
			}
			if (InputSystem::isKeyUp(KeyMap::vk_Left) || InputSystem::isKeyUp(KeyMap::vk_A))
			{
				m_movementAxis.x += 1;
			}

			if (InputSystem::isKeyDown(KeyMap::vk_Down) || InputSystem::isKeyDown(KeyMap::vk_S))
			{
				m_movementAxis.y += 1;
			}
			if (InputSystem::isKeyUp(KeyMap::vk_Down) || InputSystem::isKeyUp(KeyMap::vk_S))
			{
				m_movementAxis.y -= 1;
			}

			if (InputSystem::isKeyDown(KeyMap::vk_Right) || InputSystem::isKeyDown(KeyMap::vk_D))
			{
				m_movementAxis.x += 1;
			}
			if (InputSystem::isKeyUp(KeyMap::vk_Right) || InputSystem::isKeyUp(KeyMap::vk_D))
			{
				m_movementAxis.x -= 1;
			}

			if (InputSystem::isKeyDown(KeyMap::vk_Space))
			{
				using namespace events;
				PlaySoundData* data = new PlaySoundData();
				data->path = "Media/Audio/slap.wav";
				EventSystem::fireEvent("PlaySound", data);
			}

			if (InputSystem::isKeyUp(KeyMap::vk_P))
			{
				int a = 0;
			}

			if (m_movementAxis.x != 0 || m_movementAxis.y != 0)
			{
				glm::vec3 movement = glm::vec3(playerSpeed * deltaTime * -m_movementAxis.x, 0.0f, playerSpeed * deltaTime * -m_movementAxis.y);

				// --------------------
				//glm::vec3 forward = cameraTransform.rotation * glm::vec3(0, 0, 1);
				//glm::vec3 right = cameraTransform.rotation * glm::vec3(1, 0, 0);
				////forward.y = 0;
				////right.y = 0;
				//forward = glm::normalize(forward);
				//right = glm::normalize(right);
				//movement = forward * movement.z + right * movement.x;
				// --------------------

				cameraTransform->position += cameraComp.right * movement.x;
				cameraTransform->position += cameraComp.forward * movement.z;

				//m_eventManager->FireEvent<MoveEvent>(new MoveEvent(playerComp.getEntityId(), movement));
				//printf("position: x %f, y %f, z %f\n", playerTransform.getPosition().x, playerTransform.getPosition().y, playerTransform.getPosition().z);
			}
		}
	}
}