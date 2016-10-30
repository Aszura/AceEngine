/* Shader.h - Definitions for shader loading and compilation */
#pragma once

#include <string>
#include <map>

namespace rendering
{
	struct Shader
	{
		std::map<const char*, unsigned int>	uniformLocations;
		std::map<const char*, unsigned int>	uniformBlockIndices;
		std::string							path;
		unsigned int						id;
		unsigned int						vertexShader;
		unsigned int						fragmentShader;
	};
}