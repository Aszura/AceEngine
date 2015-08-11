#include "PhysicsSystem.h"
#include "EntityWorld.h"
#include "Texture.h"

#include <phsyx\PxPhysicsAPI.h>
#include <phsyx\extensions\PxDefaultSimulationFilterShader.h>
#include <assert.h>

PhysicsSystem::PhysicsSystem(EntityWorld* entityWorld)
	: mEntityWorld(entityWorld)
	, mNbThreads(1)
	, mAccumulator(0.0f)
	, mStepSize(1.0f / 60.0f)
{
	assert(entityWorld);
}

PhysicsSystem::~PhysicsSystem()
{
	mControllerManager->release();
	mPhysics->release();
	mCooking->release();
	mFoundation->release();
}

void PhysicsSystem::start()
{
	using namespace physx;

	static PxDefaultErrorCallback gDefaultErrorCallback; 
	static PxDefaultAllocator gDefaultAllocatorCallback;
	static PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation)
	{
		printf("PxCreateFoundation failed!");
	}

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));
	if (!mCooking)
		printf("PxCreateCooking failed!");

	mPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale());
	if (!mPhysics)
	{
		printf("PxCreatePhysics failed!");
	}

	PxRegisterUnifiedHeightFields(*mPhysics);

	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -19.62f, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		mCpuDispatcher = PxDefaultCpuDispatcherCreate(mNbThreads);
		if (!mCpuDispatcher)
			printf("PxDefaultCpuDispatcherCreate failed!");
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}
	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = gDefaultFilterShader;

	//#ifdef PX_WINDOWS
	//	if (!sceneDesc.gpuDispatcher && mCudaContextManager)
	//	{
	//		sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
	//	}
	//#endif

	mScene = mPhysics->createScene(sceneDesc);
	if (!mScene)
		printf("createScene failed!");

	mControllerManager = PxCreateControllerManager(*mScene);

	for (auto& charControllerComp : mEntityWorld->getCharacterControllerWorld().getArray())
	{
		PxCapsuleControllerDesc desc;
		desc.material = mPhysics->createMaterial(0.5f, 0.5f, 0.1f);
		desc.height = charControllerComp.height;
		desc.radius = charControllerComp.radius;
		desc.slopeLimit = charControllerComp.slopeLimit;

		bool b = desc.isValid();

		PxController* c = mControllerManager->createController(desc);
		charControllerComp.controller = c;
	}

	for (auto& terrainComp : mEntityWorld->getTerrainWorld().getArray())
	{
		TransformComponent* transformComp = mEntityWorld->getTransformWorld().getFirst(terrainComp.entityId);
		assert(transformComp);

		unsigned int size = terrainComp.heightTexture->size.x * terrainComp.heightTexture->size.y;
		PxHeightFieldSample* samples = new PxHeightFieldSample[size];

		for (size_t i = 0; i < size; i++)
		{
			samples[i].height = static_cast<physx::PxI16>((terrainComp.heightTexture->image.getPixelsPtr()[i * 4] / 255.0f) * terrainComp.maxHeight);
			samples[i].materialIndex0 = 0;
			samples[i].materialIndex1 = 0;
			//samples[i].setTessFlag();
		}

		PxHeightFieldDesc hfDesc;
		hfDesc.format = PxHeightFieldFormat::eS16_TM;
		hfDesc.nbColumns = terrainComp.heightTexture->size.x;
		hfDesc.nbRows = terrainComp.heightTexture->size.y;
		hfDesc.thickness = -10.0f;
		hfDesc.samples.data = samples;
		hfDesc.samples.stride = sizeof(PxHeightFieldSample);

		PxHeightField* aHeightField = mCooking->createHeightField(hfDesc, mPhysics->getPhysicsInsertionCallback());
		PxHeightFieldGeometry* hfGeom = new PxHeightFieldGeometry(aHeightField, PxMeshGeometryFlags(), 1.0f, terrainComp.terrainScale, terrainComp.terrainScale);
		PxRigidStatic* heightFieldActor = mPhysics->createRigidStatic(PxTransform(transformComp->position.x, transformComp->position.y, transformComp->position.z));

		PxShape* aHeightFieldShape = heightFieldActor->createShape(*hfGeom, *mPhysics->createMaterial(0.5f, 0.5f, 0.1f));
		mScene->addActor(*heightFieldActor);
	}

	for (auto& capsuleColliderComp : mEntityWorld->getCapsuleColliderWorld().getArray())
	{
		TransformComponent* transformComp = mEntityWorld->getTransformWorld().getFirst(capsuleColliderComp.entityId);
		assert(transformComp);

		PxMaterial* material;
		material = mPhysics->createMaterial(0.5f, 0.5f, 0.1f);    //static friction, dynamic friction, restitution

		if (!material)
		{
			printf("entityId %u:createMaterial failed!", capsuleColliderComp.entityId);
		}
		capsuleColliderComp.material = material;

		if (mEntityWorld->getCapsuleColliderWorld().exist(capsuleColliderComp.entityId))
		{
			RigidbodyComponent* rigidbodyComp = mEntityWorld->getRigidbodyWorld().getFirst(capsuleColliderComp.entityId);
			assert(rigidbodyComp);

			PxRigidDynamic* capsuleActor = mPhysics->createRigidDynamic(PxTransform(transformComp->position.x, transformComp->position.y, transformComp->position.z));
			PxShape* capsuleShape = capsuleActor->createShape(PxCapsuleGeometry(capsuleColliderComp.radius, capsuleColliderComp.height / 2.0f), *material);
			PxRigidBodyExt::updateMassAndInertia(*capsuleActor, 1.0f);

			capsuleColliderComp.shape = capsuleShape;
			rigidbodyComp->rigidbody = capsuleActor;
			mScene->addActor(*capsuleActor);
		}
		else
		{
			PxRigidStatic* capsuleActor = mPhysics->createRigidStatic(PxTransform(transformComp->position.x, transformComp->position.y, transformComp->position.z));
			PxShape* capsuleShape = capsuleActor->createShape(PxCapsuleGeometry(capsuleColliderComp.radius, capsuleColliderComp.height / 2.0f), *material);
			capsuleColliderComp.shape = capsuleShape;
			capsuleColliderComp.staticBody = capsuleActor;
			mScene->addActor(*capsuleActor);
		}
	}
}

void PhysicsSystem::update(float deltaTime)
{
	for (auto& charControllerComp : mEntityWorld->getCharacterControllerWorld().getArray())
	{
		using namespace physx;

		TransformComponent* transformComp = mEntityWorld->getTransformWorld().getFirst(charControllerComp.entityId);
		assert(transformComp);

		PxRaycastBuffer hit;
		charControllerComp.isOnGround = mScene->raycast(
			PxVec3(transformComp->position.x, transformComp->position.y - (charControllerComp.height / 2.0f + 0.5f), transformComp->position.z),
			PxVec3(0.0f, -1.0f, 0.0f),
			1.0f,
			hit
			);

		charControllerComp.force += glm::vec3(0.0f, mScene->getGravity().y * deltaTime, 0.0f); //Apply gravity
		charControllerComp.velocity += charControllerComp.force; // Apply impulse force
		charControllerComp.velocity.y = glm::max(charControllerComp.velocity.y, -25.0f);
		charControllerComp.force = glm::vec3(0.0f);
		transformComp->position += charControllerComp.velocity * deltaTime;

		//printf("Velocity: %f\n", charControllerComp.velocity.y);

		//Update position
		charControllerComp.controller->move(
			PxExtendedVec3(transformComp->position.x, transformComp->position.y, transformComp->position.z) - charControllerComp.controller->getPosition(),
			0.01f,
			deltaTime,
			PxControllerFilters()
			);

		PxExtendedVec3 newPos = charControllerComp.controller->getPosition();
		transformComp->position = glm::vec3(newPos.x, newPos.y, newPos.z);
	}

	mAccumulator += deltaTime;

	while (mAccumulator >= mStepSize)
	{
		for (auto& rigidbodyComp : mEntityWorld->getRigidbodyWorld().getArray())
		{
			using namespace physx;

			TransformComponent* transformComp = mEntityWorld->getTransformWorld().getFirst(rigidbodyComp.entityId);
			assert(transformComp);
			rigidbodyComp.rigidbody->setGlobalPose(PxTransform(transformComp->position.x, transformComp->position.y, transformComp->position.z, PxQuat(1.0f)));
		}

		mScene->simulate(mStepSize);
		mScene->fetchResults(true);

		for (auto& rigidbodyComp : mEntityWorld->getRigidbodyWorld().getArray())
		{
			using namespace physx;

			TransformComponent* transformComp = mEntityWorld->getTransformWorld().getFirst(rigidbodyComp.entityId);
			assert(transformComp);

			physx::PxTransform& physxTransform = rigidbodyComp.rigidbody->getGlobalPose();
			transformComp->position = glm::vec3(physxTransform.p.x, physxTransform.p.y, physxTransform.p.z);
			rigidbodyComp.rigidbody->setGlobalPose(PxTransform(transformComp->position.x, transformComp->position.y, transformComp->position.z, PxQuat(1.0f)));
			physxTransform = rigidbodyComp.rigidbody->getGlobalPose();
		}

		mAccumulator -= mStepSize;
	}
}