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

namespace component
{
	class EntityWorld;
}

namespace physics
{
	class PhysicsSystem
	{
	public:
		PhysicsSystem(component::EntityWorld* entityWorld);
		~PhysicsSystem();

		void start();
		void update(float deltaTime);
	private:
		component::EntityWorld*			mEntityWorld;
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
}