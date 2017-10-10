#pragma once
#include <glm\glm.hpp>
#include <iostream>
#include <fstream>
#include <string>

namespace utility
{
	bool pointInRect(glm::vec2 point, glm::vec2 rectPos, glm::vec2 rectSize)
	{
		if (point.x < rectPos.x ||
			point.y < rectPos.y ||
			point.x >= rectPos.x + rectSize.x ||
			point.y >= rectPos.y + rectSize.y)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	//helper funktion - Read shader file with string
	std::string textFileRead(const std::string& fileName)
	{
		std::ifstream file (fileName, std::ifstream::in);

		if (file)
		{
			std::string outputString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
			file.close();
			return outputString;
		}

		return std::string();
	}
}