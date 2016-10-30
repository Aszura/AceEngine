#include "SceneLoader.h"
#include "ComponentSerializer.h"
#include "SceneImporter.h"
#include "CustomTypes.h"
#include "EntityWorld.h"
#include "Texture.h"
#include "ResourceLoader.h"

#include <assert.h>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <rapidxml\rapidxml.hpp>

namespace component
{
	namespace serialization
	{
		SceneLoader::SceneLoader(EntityWorld* entityWorld, utility::ResourceLoader<rendering::Texture>* textureLoader, utility::ResourceLoader<rendering::Shader>* shaderLoader)
			: mEntityWorld(entityWorld)
			, mTextureLoader(textureLoader)
			, mShaderLoader(shaderLoader)
		{
			assert(entityWorld);
			assert(textureLoader);
			assert(shaderLoader);
		}

		SceneLoader::~SceneLoader()
		{
		}

		bool SceneLoader::loadScene(const std::string& filePath)
		{
			using namespace rapidxml;

			std::ifstream fileStream(filePath);
			std::string sceneText; //Make sure memory is located on heap to avoid stack overflow

			fileStream.seekg(0, std::ios::end);
			sceneText.reserve(static_cast<unsigned int>(fileStream.tellg()));
			fileStream.seekg(0, std::ios::beg);

			sceneText.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

			xml_document<>* doc = new xml_document<>();
			doc->parse<0>(&sceneText[0]);

			//Load components
			ComponentSerializer serializer = ComponentSerializer(mEntityWorld, mTextureLoader, mShaderLoader);
			xml_node<>* componentParentNode = doc->first_node("Components");

			for (xml_node<>* node = componentParentNode->first_node(); node != nullptr; node = node->next_sibling())
			{
				std::unordered_map<std::string, std::string> data;

				for (xml_attribute<>* attribute = node->first_attribute(); attribute != nullptr; attribute = attribute->next_attribute())
				{
					data[attribute->name()] = attribute->value();
				}

				serializer.deserialize(node->name(), data);
			}

			//Load models
			rendering::SceneImporter sceneImporter = rendering::SceneImporter(mEntityWorld, mTextureLoader, mShaderLoader);
			xml_node<>* modelsParentNode = doc->first_node("Models");

			for (xml_node<>* node = modelsParentNode->first_node(); node != nullptr; node = node->next_sibling())
			{
				std::unordered_map<std::string, std::string> data;

				for (xml_attribute<>* attribute = node->first_attribute(); attribute != nullptr; attribute = attribute->next_attribute())
				{
					data[attribute->name()] = attribute->value();
				}

				EntityId entityId;
				std::stringstream strStream;
				strStream << data["entityId"];
				strStream >> entityId;

				sceneImporter.loadScene(entityId, data["scenePath"].c_str(), data["shader"]);

				if (data.count("materialTexture") > 0)
				{
					MeshComponent* meshComp = mEntityWorld->getMeshWorld().getFirst(entityId);
					meshComp->material->texture = mTextureLoader->load(data["materialTexture"]);
				}

				if (data.count("materialNormalTexture") > 0)
				{
					MeshComponent* meshComp = mEntityWorld->getMeshWorld().getFirst(entityId);
					meshComp->material->normalTexture = mTextureLoader->load(data["materialNormalTexture"]);
				}
			}

			delete doc;
			mCurrentScene = filePath;

			return true;
		}
	}
}