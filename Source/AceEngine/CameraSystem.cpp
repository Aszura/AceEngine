#include "CameraSystem.h"
#include "InputSystem.h"
#include "EntityWorld.h"
#include "EventSystem.h"

#include <glm\ext.hpp>

CameraSystem::CameraSystem(EntityWorld* entityWorld)
	: mEntityWorld(entityWorld)
{
	assert(entityWorld);
}


CameraSystem::~CameraSystem()
{
}

void CameraSystem::start()
{
}

void CameraSystem::update(float deltaTime)
{
	const float playerSpeed = 10.0f;
	const float lookSensitivity = 10.30f;
	const float jumpForce = 35.0f;


	if (mEntityWorld->getCameraWorld().getArray().length() > 0)
	{
		CameraComponent& cameraComp = *mEntityWorld->getCameraWorld().getArray().begin();
		PlayerComponent* playerComp = &*mEntityWorld->getPlayerWorld().getArray().begin();
		assert(playerComp);
		TransformComponent* cameraTransform = mEntityWorld->getTransformWorld().getFirst(cameraComp.entityId);
		assert(cameraTransform);
		CharacterControllerComponent* charControllerComp = mEntityWorld->getCharacterControllerWorld().getFirst(cameraComp.entityId);
		assert(charControllerComp);

		if (InputSystem::isKeyDown(InputSystem::Key::Up) || InputSystem::isKeyDown(InputSystem::Key::W))
		{
			mMovementAxis.y -= 1;
		}
		if (InputSystem::isKeyUp(InputSystem::Key::Up) || InputSystem::isKeyUp(InputSystem::Key::W))
		{
			mMovementAxis.y += 1;
		}

		if (InputSystem::isKeyDown(InputSystem::Key::Left) || InputSystem::isKeyDown(InputSystem::Key::A))
		{
			mMovementAxis.x -= 1;
		}
		if (InputSystem::isKeyUp(InputSystem::Key::Left) || InputSystem::isKeyUp(InputSystem::Key::A))
		{
			mMovementAxis.x += 1;
		}

		if (InputSystem::isKeyDown(InputSystem::Key::Down) || InputSystem::isKeyDown(InputSystem::Key::S))
		{
			mMovementAxis.y += 1;
		}
		if (InputSystem::isKeyUp(InputSystem::Key::Down) || InputSystem::isKeyUp(InputSystem::Key::S))
		{
			mMovementAxis.y -= 1;
		}

		if (InputSystem::isKeyDown(InputSystem::Key::Right) || InputSystem::isKeyDown(InputSystem::Key::D))
		{
			mMovementAxis.x += 1;
		}
		if (InputSystem::isKeyUp(InputSystem::Key::Right) || InputSystem::isKeyUp(InputSystem::Key::D))
		{
			mMovementAxis.x -= 1;
		}

		if (InputSystem::isKeyDown(InputSystem::Key::Space) && charControllerComp->isOnGround)
		{
			charControllerComp->force += glm::vec3(0.0f, jumpForce, 0.0f);

			using namespace events;
			PlaySoundData* data = new PlaySoundData();
			data->path = "../../Media/Audio/slap.wav";
			EventSystem::fireEvent(EventType::PlaySound, data);
		}


		if (glm::abs(InputSystem::getMouseDelta().x) > 0.0001f || glm::abs(InputSystem::getMouseDelta().y) > 0.0001f)
		{
			glm::vec3 amount = glm::vec3(-InputSystem::getMouseDelta().x, InputSystem::getMouseDelta().y, 0) * lookSensitivity * deltaTime;
			amount = glm::radians(amount);

			// ---------
			cameraComp.yaw += amount.x;
			cameraComp.pitch += amount.y;

			glm::mat4 rotMat = glm::yawPitchRoll(cameraComp.yaw, cameraComp.pitch, cameraComp.roll);
			cameraComp.direction = glm::vec3(rotMat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
			cameraComp.direction = glm::normalize(cameraComp.direction);

			glm::mat4 rotateYTempMatrix;
			rotateYTempMatrix = glm::rotate(rotateYTempMatrix, cameraComp.yaw, glm::vec3(0.0f, 1.0f, 0.0f));
			cameraComp.right = glm::vec3(rotateYTempMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			cameraComp.up = glm::vec3(rotateYTempMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			cameraComp.forward = glm::vec3(rotateYTempMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

			// ---------

			// ------------------
			////Pitch
			//glm::quat q = glm::angleAxis(amount.y, cameraComp.right);
			//cameraComp.up = glm::rotate(q, cameraComp.up);
			//cameraComp.forward = glm::rotate(q, cameraComp.forward);
			//cameraTransform.rotation = q * cameraTransform.rotation;

			////Yaw
			//q = angleAxis(amount.x, cameraComp.up);
			//cameraComp.right = glm::rotate(q, cameraComp.right);
			//cameraComp.forward = glm::rotate(q, cameraComp.forward);
			//cameraTransform.rotation = q * cameraTransform.rotation;

			// ------------------

			/*glm::quat rotation = playerTransform.rotation;
			rotation = glm::normalize(glm::angleAxis(amount.y, glm::vec3(0, 1, 0))) * rotation;
			playerTransform.rotation = rotation;*/

			//glm::vec3 localRight = glm::normalize(rotation * glm::vec3(1, 0, 0));
			//glm::quat newRotation = glm::normalize(glm::angleAxis(amount.x, localRight)) * rotation;
			//glm::quat rotationChange = glm::normalize(glm::angleAxis(amount.x, glm::vec3(1, 0, 0)));
			//glm::quat cameraRotation = rotationChange * cameraTransform.rotation;
			//cameraTransform.rotation = cameraRotation;
			//cameraTransform.position = rotationChange * cameraTransform.position;
		}

		if (InputSystem::isKeyUp(InputSystem::Key::P))
		{
			int a = 0;
		}

		if (mMovementAxis.x != 0 || mMovementAxis.y != 0)
		{
			glm::vec3 movement = glm::vec3(playerSpeed * deltaTime * -mMovementAxis.x, 0.0f, playerSpeed * deltaTime * -mMovementAxis.y);

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