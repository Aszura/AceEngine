#pragma once
#include "CustomTypes.h"
#include "Material.h"

#include <glm\glm.hpp>
#include <vector>

class btCollisionShape;

namespace rendering
{
	struct Texture;

	struct TerrainVertexData
	{
		glm::vec2 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};
}

namespace component
{
	struct TerrainComponent
	{
		std::vector<rendering::TerrainVertexData>	terrainData;
		std::vector<unsigned int>					terrainIndices;
		rendering::Texture*							heightTexture = nullptr;
		rendering::Material*						material;
		btCollisionShape*							collisionShape = nullptr;
		component::EntityId							entityId;
		component::ElementId						componentId;
		unsigned int								vao;
		unsigned int								vbo;
		unsigned int								ibo;
		unsigned int								indicesNum = 0;
		unsigned int								tiling = 20;
		float										maxHeight = 155.0f;
		float										terrainScale = 1.0f;
	};
}