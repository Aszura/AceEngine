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
		ComponentSerializer::ComponentSerializer(EntityWorld* entityWorld, utility::ResourceLoader<rendering::Texture>* textureLoader, utility::ResourceLoader<rendering::Shader>* shaderLoader)
			: mEntityWorld(entityWorld)
			, mTextureLoader(textureLoader)
			, mShaderLoader(shaderLoader)
		{
			assert(entityWorld);
			assert(textureLoader);
			assert(shaderLoader);
		}

		void ComponentSerializer::deserialize(const std::string typeName, std::unordered_map<std::string, std::string>& members)
		{
			assert(members.count("entityId") > 0);
			EntityId entityId = extractValue<EntityId>(members["entityId"]);

			if (typeName == "TransformComponent")
			{
				TransformComponent* transformComp = mEntityWorld->getTransformWorld().add(entityId);

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
			}
			else if (typeName == "CameraComponent")
			{
				CameraComponent* cameraComp = mEntityWorld->getCameraWorld().add(entityId);

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
			}
			else if (typeName == "CapsuleColliderComponent")
			{
				CapsuleColliderComponent* capsuleComp = mEntityWorld->getCapsuleColliderWorld().add(entityId);

				loadValue(members, "height", capsuleComp->height);
				loadValue(members, "radius", capsuleComp->radius);
				loadValue(members, "isDynamic", capsuleComp->isDynamic);
			}
			else if (typeName == "CharacterControllerComponent")
			{
				CharacterControllerComponent* characterComp = mEntityWorld->getCharacterControllerWorld().add(entityId);

				loadValue(members, "height", characterComp->height);
				loadValue(members, "radius", characterComp->radius);
				loadValue(members, "slopeLimit", characterComp->slopeLimit);
			}
			else if (typeName == "LightComponent")
			{
				LightComponent* lightComp = mEntityWorld->getLightWorld().add(entityId);

				loadValue(members, "intensity", lightComp->intensity);

				loadValue(members, "colorX", lightComp->color.x);
				loadValue(members, "colorY", lightComp->color.y);
				loadValue(members, "colorZ", lightComp->color.z);
				loadValue(members, "colorW", lightComp->color.w);
			}
			else if (typeName == "MusicComponent")
			{
				MusicComponent* musicComp = mEntityWorld->getMusicWorld().add(entityId);

				loadValue(members, "isLooping", musicComp->isLooping);
				loadValue(members, "isPlaying", musicComp->isPlaying);
				loadValue(members, "filePath", musicComp->filePath);
			}
			else if (typeName == "PlayerComponent")
			{
				mEntityWorld->getPlayerWorld().add(entityId);
			}
			else if (typeName == "RigidbodyComponent")
			{
				mEntityWorld->getRigidbodyWorld().add(entityId);
			}
			else if (typeName == "SkyboxComponent")
			{
				SkyboxComponent* skyboxComp = mEntityWorld->getSkyboxWorld().add(entityId);

				std::string texturePaths[6];

				if (loadValue(members, "textures0", texturePaths[0]))
				{
					skyboxComp->textures[0] = mTextureLoader->load(texturePaths[0]);
				}
				if (loadValue(members, "textures1", texturePaths[1]))
				{
					skyboxComp->textures[1] = mTextureLoader->load(texturePaths[1]);
				}
				if (loadValue(members, "textures2", texturePaths[2]))
				{
					skyboxComp->textures[2] = mTextureLoader->load(texturePaths[2]);
				}
				if (loadValue(members, "textures3", texturePaths[3]))
				{
					skyboxComp->textures[3] = mTextureLoader->load(texturePaths[3]);
				}
				if (loadValue(members, "textures4", texturePaths[4]))
				{
					skyboxComp->textures[4] = mTextureLoader->load(texturePaths[4]);
				}
				if (loadValue(members, "textures5", texturePaths[5]))
				{
					skyboxComp->textures[5] = mTextureLoader->load(texturePaths[5]);
				}

				skyboxComp->textures[0]->type = rendering::TextureType::CubemapPosX;
				skyboxComp->textures[1]->type = rendering::TextureType::CubemapNegX;
				skyboxComp->textures[2]->type = rendering::TextureType::CubemapPosY;
				skyboxComp->textures[3]->type = rendering::TextureType::CubemapNegY;
				skyboxComp->textures[4]->type = rendering::TextureType::CubemapPosZ;
				skyboxComp->textures[5]->type = rendering::TextureType::CubemapNegZ;
			}
			else if (typeName == "TerrainComponent")
			{
				TerrainComponent* terrainComp = mEntityWorld->getTerrainWorld().add(entityId);

				loadValue(members, "maxHeight", terrainComp->maxHeight);
				loadValue(members, "terrainScale", terrainComp->terrainScale);
				loadValue(members, "tiling", terrainComp->tiling);

				std::string heightTexturePath;

				if (loadValue(members, "heightTexture", heightTexturePath))
				{
					terrainComp->heightTexture = mTextureLoader->load(heightTexturePath);
					terrainComp->heightTexture->sRGB = false;
				}

				//Material
				terrainComp->material = new rendering::Material();

				std::string texturePath;

				if (loadValue(members, "materialTexture", texturePath))
				{
					terrainComp->material->texture = mTextureLoader->load(texturePath);
				}

				std::string normalTexturePath;

				if (loadValue(members, "normalTexture", normalTexturePath))
				{
					terrainComp->material->normalTexture = mTextureLoader->load(normalTexturePath);
					terrainComp->material->normalTexture->sRGB = false;
				}

				std::string shaderPath;

				if (loadValue(members, "materialShader", shaderPath))
				{
					terrainComp->material->shader = mShaderLoader->load(shaderPath);
				}

				loadValue(members, "materialShininess", terrainComp->material->shininess);

				loadValue(members, "materialColorX", terrainComp->material->color.x);
				loadValue(members, "materialColorY", terrainComp->material->color.y);
				loadValue(members, "materialColorZ", terrainComp->material->color.z);
				loadValue(members, "materialColorW", terrainComp->material->color.w);

				loadValue(members, "materialSpecularX", terrainComp->material->specular.x);
				loadValue(members, "materialSpecularY", terrainComp->material->specular.y);
				loadValue(members, "materialSpecularZ", terrainComp->material->specular.z);
				loadValue(members, "materialSpecularW", terrainComp->material->specular.w);

				loadValue(members, "materialAmbientX", terrainComp->material->ambient.x);
				loadValue(members, "materialAmbientY", terrainComp->material->ambient.y);
				loadValue(members, "materialAmbientZ", terrainComp->material->ambient.z);
				loadValue(members, "materialAmbientW", terrainComp->material->ambient.w);
			}
		}
	}
}