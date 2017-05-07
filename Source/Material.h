#pragma once
#include <glm\glm.hpp>

namespace rendering
{
	struct Texture;
	struct Shader;

	struct Material
	{
		Texture*	texture = nullptr;
		Shader* 	shader;
		glm::vec4	color = glm::vec4(1.0f);
	};
}