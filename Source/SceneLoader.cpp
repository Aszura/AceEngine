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

SceneLoader::SceneLoader(EntityWorld* entityWorld, ResourceLoader<Texture>* textureLoader, ResourceLoader<Shader>* shaderLoader)
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
	std::string sceneText;

	fileStream.seekg(0, std::ios::end);
	sceneText.reserve(static_cast<unsigned int>(fileStream.tellg()));
	fileStream.seekg(0, std::ios::beg);

	sceneText.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

	xml_document<> doc;
	doc.parse<0>(&sceneText[0]);

	//Load components
	ComponentSerializer serializer = ComponentSerializer(mEntityWorld, mTextureLoader, mShaderLoader);
	xml_node<>* componentParentNode = doc.first_node("Components");

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
	SceneImporter sceneImporter = SceneImporter(mEntityWorld, mTextureLoader, mShaderLoader);
	xml_node<>* modelsParentNode = doc.first_node("Models");

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

	mCurrentScene = filePath;

	return true;
}