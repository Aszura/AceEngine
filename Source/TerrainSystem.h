#pragma once
#include <glm\glm.hpp>

class EntityWorld;

class TerrainSystem
{
public:
	TerrainSystem(EntityWorld* entityWorld);
	~TerrainSystem();

	void start();
private:
	EntityWorld*	mEntityWorld;
};

