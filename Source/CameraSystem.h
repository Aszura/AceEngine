#pragma once
#include <glm\glm.hpp>

namespace component
{
	class EntityWorld;
}

namespace rendering
{
	class CameraSystem
	{
	public:
		explicit CameraSystem(component::EntityWorld& entityWorld);
		~CameraSystem() = default;

		void start();
		void update(float deltaTime);
	private:
		component::EntityWorld& m_entityWorld; 
		glm::ivec2 m_movementAxis;
	};
}
