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
		CameraSystem(component::EntityWorld* entityWorld);
		~CameraSystem();

		void start();
		void update(float deltaTime);
	private:
		component::EntityWorld* mEntityWorld;
		glm::ivec2 mMovementAxis;
	};
}
