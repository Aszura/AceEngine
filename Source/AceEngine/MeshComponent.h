#pragma once
#include "CustomTypes.h"
#include "VertexData.h"
#include "Material.h"
#include <glm\glm.hpp>
#include <vector>

struct MeshComponent
{
	std::vector<VertexData>		vertices;
	std::vector<glm::uvec3>		indices;
	Material*					material;
	EntityId					entityId;
	ElementId					componentId;
	unsigned int				vao;
	unsigned int				vbo;
	unsigned int				ibo;
};