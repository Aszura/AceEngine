#include "SceneLoader.h"
#include "ComponentSerializer.h"
#include "CustomTypes.h"
#include "EntityWorld.h"
#include "Texture.h"
#include "ResourceLoader.h"
#include "Shader.h"

#include <assert.h>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <rapidxml\rapidxml.hpp>

namespace component
{
	namespace serialization
	{
		SceneLoader::SceneLoader(EntityWorld& entityWorld, utility::ResourceLoader<rendering::Texture>& textureLoader, utility::ResourceLoader<rendering::Shader>& shaderLoader)
			: m_entityWorld(entityWorld)
			, m_textureLoader(textureLoader)
			, m_shaderLoader(shaderLoader)
		{
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
			ComponentSerializer serializer = ComponentSerializer(m_entityWorld, m_textureLoader, m_shaderLoader);
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

			delete doc;
			m_currentScene = filePath;

			return true;
		}
	}
}