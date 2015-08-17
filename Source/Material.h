#pragma once
#include <glm\glm.hpp>

struct Texture;
struct Shader;

struct Material
{
	Texture*	texture = nullptr;
	Texture*	normalTexture = nullptr;
	Shader* 	shader;
	glm::vec4	color = glm::vec4(1.0f);
	glm::vec4	specular = glm::vec4(0.5f);
	glm::vec4	ambient;
	float		shininess;
};