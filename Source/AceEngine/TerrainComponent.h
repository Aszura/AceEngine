#pragma once
#include "CustomTypes.h"
#include "Material.h"

#include <glm\glm.hpp>
#include <vector>

struct Texture;
class btCollisionShape;

struct TerrainVertexData
{
	glm::vec2 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct TerrainComponent
{
	std::vector<TerrainVertexData>	terrainData;
	std::vector<unsigned int>		terrainIndices;
	Texture*						heightTexture = nullptr;
	Material*						material;
	btCollisionShape*				collisionShape = nullptr;
	EntityId						entityId;
	ElementId						componentId;
	unsigned int					vao;
	unsigned int					vbo;
	unsigned int					ibo;
	unsigned int					indicesNum = 0;
	unsigned int					tiling = 20;
	float							maxHeight = 155.0f;
	float							terrainScale = 1.0f;
};