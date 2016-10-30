#pragma once
#include <glm\glm.hpp>

namespace component
{
	class EntityWorld;
}

namespace rendering
{
	class TerrainSystem
	{
	public:
		TerrainSystem(component::EntityWorld* entityWorld);
		~TerrainSystem();

		void start();
	private:
		component::EntityWorld*	mEntityWorld;
	};
}