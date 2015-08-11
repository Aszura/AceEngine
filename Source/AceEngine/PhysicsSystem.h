#pragma once
#include <glm\glm.hpp>

namespace physx
{
	class PxFoundation;
	class PxCooking;
	class PxPhysics;
	class PxScene;
	class PxDefaultCpuDispatcher;
	class PxControllerManager;
}

class EntityWorld;

class PhysicsSystem
{
public:
	PhysicsSystem(EntityWorld* entityWorld);
	~PhysicsSystem();

	void start();
	void update(float deltaTime);
private:
	EntityWorld*					mEntityWorld;
	physx::PxFoundation*			mFoundation;
	physx::PxPhysics*				mPhysics;
	physx::PxScene*					mScene;
	physx::PxDefaultCpuDispatcher*	mCpuDispatcher;
	physx::PxCooking*				mCooking;
	physx::PxControllerManager*		mControllerManager;
	unsigned int					mNbThreads;
	float							mAccumulator;
	float							mStepSize;
};

