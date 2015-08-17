#pragma once
#include <glm\glm.hpp>
#include <iostream>
#include <string>

namespace utilities
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

	//helper funktion - Read shader file
	char* textFileRead(const char *fileName)
	{
		char* text;

		if (fileName != NULL) 
		{
			FILE *file;
			fopen_s(&file, fileName, "rt");

			if (file != NULL) 
			{
				fseek(file, 0, SEEK_END);
				int count = ftell(file);
				rewind(file);

				if (count > 0) 
				{
					text = (char*)malloc(sizeof(char) * (count + 1));
					count = fread(text, sizeof(char), count, file);
					text[count] = '\0';
				}

				fclose(file);
			}
		}

		return text;
	}

	//helper funktion - Read shader file with string
	char* textFileRead(const std::string& fileName)
	{
		char* text;

		if (!fileName.empty()) 
		{
			FILE *file;
			fopen_s(&file, fileName.c_str(), "rt");

			if (file != NULL) 
			{
				fseek(file, 0, SEEK_END);
				int count = ftell(file);
				rewind(file);

				if (count > 0) 
				{
					text = (char*)malloc(sizeof(char) * (count + 1));
					count = fread(text, sizeof(char), count, file);
					text[count] = '\0';
				}

				fclose(file);
			}
		}

		return text;
	}
}