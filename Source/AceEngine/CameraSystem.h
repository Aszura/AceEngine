#pragma once
#include <glm\glm.hpp>

class EntityWorld;

class CameraSystem
{
public:
	CameraSystem(EntityWorld* entityWorld);
	~CameraSystem();

	void start();
	void update(float deltaTime);
private:
	EntityWorld* mEntityWorld;
	glm::ivec2 mMovementAxis;
};

