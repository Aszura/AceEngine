#include "ComponentSerializer.h"
#include "EntityWorld.h"
#include "CustomTypes.h"
#include "ResourceLoader.h"
#include "Texture.h"
#include "Shader.h"

namespace component
{
	namespace serialization
	{
		ComponentSerializer::ComponentSerializer(EntityWorld& entityWorld, utility::ResourceLoader<rendering::Texture>& textureLoader, utility::ResourceLoader<rendering::Shader>& shaderLoader)
			: m_entityWorld(entityWorld)
			, m_textureLoader(textureLoader)
			, m_shaderLoader(shaderLoader)
		{
			addDeserializer("TransformComponent", [this](EntityId entityId, std::unordered_map<std::string, std::string>& members) {
				TransformComponent* transformComp = m_entityWorld.getTransformWorld().add(entityId);
				assert(transformComp != nullptr);

				loadValue(members, "positionX", transformComp->position.x);
				loadValue(members, "positionY", transformComp->position.y);
				loadValue(members, "positionZ", transformComp->position.z);

				loadValue(members, "rotationX", transformComp->rotation.x);
				loadValue(members, "rotationY", transformComp->rotation.y);
				loadValue(members, "rotationZ", transformComp->rotation.z);
				loadValue(members, "rotationW", transformComp->rotation.w);

				loadValue(members, "scaleX", transformComp->scale.x);
				loadValue(members, "scaleY", transformComp->scale.y);
				loadValue(members, "scaleZ", transformComp->scale.z);
			});

			addDeserializer("SpriteComponent", [this](EntityId entityId, std::unordered_map<std::string, std::string>& members) {
				SpriteComponent* spriteComp = m_entityWorld.getSpriteWorld().add(entityId);
				assert(spriteComp != nullptr);

				loadValue(members, "shader", spriteComp->shaderPath);
				loadValue(members, "materialTexture", spriteComp->texturePath);
			});

			addDeserializer("CameraComponent", [this](EntityId entityId, std::unordered_map<std::string, std::string>& members) {
				CameraComponent* cameraComp = m_entityWorld.getCameraWorld().add(entityId);
				assert(cameraComp != nullptr);

				loadValue(members, "directionX", cameraComp->direction.x);
				loadValue(members, "directionY", cameraComp->direction.y);
				loadValue(members, "directionZ", cameraComp->direction.z);

				loadValue(members, "forwardX", cameraComp->forward.x);
				loadValue(members, "forwardY", cameraComp->forward.y);
				loadValue(members, "forwardZ", cameraComp->forward.z);

				loadValue(members, "rightX", cameraComp->right.x);
				loadValue(members, "rightY", cameraComp->right.y);
				loadValue(members, "rightZ", cameraComp->right.z);

				loadValue(members, "upX", cameraComp->up.x);
				loadValue(members, "upY", cameraComp->up.y);
				loadValue(members, "upZ", cameraComp->up.z);

				loadValue(members, "fov", cameraComp->fov);
				loadValue(members, "pitch", cameraComp->pitch);
				loadValue(members, "yaw", cameraComp->yaw);
				loadValue(members, "roll", cameraComp->roll);
				loadValue(members, "zFar", cameraComp->zFar);
				loadValue(members, "zNear", cameraComp->zNear);
			});

			addDeserializer("MusicComponent", [this](EntityId entityId, std::unordered_map<std::string, std::string>& members) {
				MusicComponent* musicComp = m_entityWorld.getMusicWorld().add(entityId);
				assert(musicComp != nullptr);

				loadValue(members, "isLooping", musicComp->isLooping);
				loadValue(members, "isPlaying", musicComp->isPlaying);
				loadValue(members, "filePath", musicComp->filePath);
			});
		}

		void ComponentSerializer::deserialize(const std::string typeName, std::unordered_map<std::string, std::string>& members)
		{
			assert(members.count("entityId") > 0);
			EntityId entityId = extractValue<EntityId>(members["entityId"]);

			m_deserializers[typeName](entityId, members);
		}

		void ComponentSerializer::addDeserializer(const std::string& typeName, std::function<void(EntityId, std::unordered_map<std::string, std::string>&)> deserializationFunction)
		{
			m_deserializers[typeName] = deserializationFunction;
		}
	}
}