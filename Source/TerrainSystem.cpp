#include "TerrainSystem.h"
#include "EntityWorld.h"
#include "Texture.h"
#include <glm\glm.hpp>

namespace rendering
{
	TerrainSystem::TerrainSystem(component::EntityWorld* entityWorld)
		: mEntityWorld(entityWorld)
	{
		assert(entityWorld);
	}

	TerrainSystem::~TerrainSystem()
	{

	}

	void TerrainSystem::start()
	{
		for (auto& terrainComp : mEntityWorld->getTerrainWorld().getArray())
		{
			sf::Image& heightTexture = terrainComp.heightTexture->image;
			heightTexture.loadFromFile(terrainComp.heightTexture->path);

			glm::uvec2 heightMapSize = glm::uvec2(heightTexture.getSize().x, heightTexture.getSize().y);

			std::vector<TerrainVertexData>& terrainData = terrainComp.terrainData;
			terrainData.resize(heightMapSize.x * heightMapSize.y);

			float* heightData = new float[terrainData.size()];

			std::vector<unsigned int>& terrainIndices = terrainComp.terrainIndices;
			size_t indicesNum = heightMapSize.x*(heightMapSize.y - 1) * 2 + (heightMapSize.y - 1);
			terrainIndices.reserve(indicesNum);

			for (size_t i = 0; i < terrainData.size(); i++)
			{
				glm::uvec2 coord = glm::uvec2(i % heightMapSize.x, glm::floor(i / static_cast<float>(heightMapSize.x)));
				terrainData[i].position.x = coord.y * terrainComp.terrainScale;
				terrainData[i].position.y = coord.x * terrainComp.terrainScale;
				heightData[i] = (heightTexture.getPixelsPtr()[i * 4] / 255.0f) * terrainComp.maxHeight;

				terrainData[i].uv = glm::vec2(coord.x / static_cast<float>(heightMapSize.x), coord.y / static_cast<float>(heightMapSize.y));// * 100.0f;
			}

			std::vector<glm::vec3> normals0;
			normals0.resize((heightMapSize.x - 1)*(heightMapSize.y - 1));

			std::vector<glm::vec3> normals1;
			normals1.resize((heightMapSize.x - 1)*(heightMapSize.y - 1));

			for (unsigned int y = 0; y < heightMapSize.y - 1; ++y)
			{
				for (unsigned int x = 0; x < heightMapSize.x; ++x)
				{
					if (x < heightMapSize.x - 1)
					{
						glm::vec3 vTriangle0[] =
						{
							glm::vec3(terrainData[y*heightMapSize.x + x].position.x, heightData[y*heightMapSize.x + x], terrainData[y*heightMapSize.x + x].position.y), //0
							glm::vec3(terrainData[(y + 1)*heightMapSize.x + x].position.x, heightData[(y + 1)*heightMapSize.x + x], terrainData[(y + 1)*heightMapSize.x + x].position.y), //1
							glm::vec3(terrainData[(y + 1)*heightMapSize.x + x + 1].position.x, heightData[(y + 1)*heightMapSize.x + x + 1], terrainData[(y + 1)*heightMapSize.x + x + 1].position.y), //2
						};

						glm::vec3 vTriangle1[] =
						{
							glm::vec3(terrainData[(y + 1)*heightMapSize.x + x + 1].position.x, heightData[(y + 1)*heightMapSize.x + x + 1], terrainData[(y + 1)*heightMapSize.x + x + 1].position.y), //0
							glm::vec3(terrainData[y*heightMapSize.x + x + 1].position.x, heightData[y*heightMapSize.x + x + 1], terrainData[y*heightMapSize.x + x + 1].position.y), //1
							glm::vec3(terrainData[y*heightMapSize.x + x].position.x, heightData[y*heightMapSize.x + x], terrainData[y*heightMapSize.x + x].position.y), //2
						};

						glm::vec3 vTriangleNorm0 = glm::cross(vTriangle0[0] - vTriangle0[1], vTriangle0[1] - vTriangle0[2]);
						glm::vec3 vTriangleNorm1 = glm::cross(vTriangle1[0] - vTriangle1[1], vTriangle1[1] - vTriangle1[2]);

						normals0[y*(heightMapSize.x - 1) + x] = glm::normalize(vTriangleNorm0);
						normals1[y*(heightMapSize.x - 1) + x] = glm::normalize(vTriangleNorm1);
					}

					terrainIndices.push_back((y + 1)*heightMapSize.x + x);
					terrainIndices.push_back(y*heightMapSize.x + x);
				}

				terrainIndices.push_back(indicesNum); //Add primitive restart
			}

			terrainComp.indicesNum = indicesNum;

			for (unsigned int y = 0; y < heightMapSize.y; ++y)
			{
				for (unsigned int x = 0; x < heightMapSize.x; ++x)
				{
					// Now we wanna calculate final normal for [i][j] vertex. We will have a look at all triangles this vertex is part of, and then we will make average vector
					// of all adjacent triangles' normals

					glm::vec3 vFinalNormal = glm::vec3(0.0f, 0.0f, 0.0f);

					// Look for upper-left triangles
					if (y != 0 && x != 0)
					{
						vFinalNormal += normals0[(y - 1)*(heightMapSize.x - 1) + x - 1];
						vFinalNormal += normals1[(y - 1)*(heightMapSize.x - 1) + x - 1];
					}

					// Look for upper-right triangles
					if (y != 0 && x != heightMapSize.x - 1)
					{
						vFinalNormal += normals0[(y - 1)*(heightMapSize.x - 1) + x];
					}

					// Look for bottom-right triangles
					if (y != heightMapSize.y - 1 && x != heightMapSize.x - 1)
					{
						vFinalNormal += normals0[y*(heightMapSize.x - 1) + x];
						vFinalNormal += normals1[y*(heightMapSize.x - 1) + x];
					}

					// Look for bottom-left triangles
					if (y != heightMapSize.y - 1 && x != 0)
					{
						vFinalNormal += normals1[y*(heightMapSize.x - 1) + x - 1];
					}

					vFinalNormal = glm::normalize(vFinalNormal);
					terrainData[y*heightMapSize.x + x].normal = -vFinalNormal; // Store final normal of j-th vertex in i-th row
				}
			}

			delete[] heightData;
		}
	}
}