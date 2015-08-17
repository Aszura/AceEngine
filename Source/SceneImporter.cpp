#include "SceneImporter.h"
#include "EntityWorld.h"
#include "Texture.h"
#include "ResourceLoader.h"
#include "Shader.h"
#include "Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm\glm.hpp>
#include <iostream>
#include <string>
#include <assert.h>

SceneImporter::SceneImporter(EntityWorld* entityWorld, ResourceLoader<Texture>* textureLoader, ResourceLoader<Shader>* shaderLoader)
	: mEntityWorld(entityWorld)
	, mImporter(new Assimp::Importer())
	, mTextureLoader(textureLoader)
	, mShaderLoader(shaderLoader)
{
	assert(entityWorld);
	assert(textureLoader);
	assert(shaderLoader);
}

SceneImporter::~SceneImporter()
{
	delete mImporter;
}

void SceneImporter::loadScene(unsigned int entityId, const char* scenePath, std::string shader)
{
	//Import Scene
	mScene = mImporter->ReadFile( scenePath, 
		aiProcess_Triangulate            |
		aiProcess_GenSmoothNormals		 |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_ImproveCacheLocality	 |
		aiProcess_CalcTangentSpace		 |
		aiProcess_OptimizeMeshes		 |
		aiProcess_OptimizeGraph);


	//Load each mesh
	for(unsigned int i = 0; i < mScene->mNumMeshes; ++i)
	{
		MeshComponent* meshComponent = mEntityWorld->getMeshWorld().add(entityId);
		assert(meshComponent);

		aiMesh* mesh = mScene->mMeshes[i];
		assert(mesh);
		aiMaterial* material = mScene->mMaterials[mesh->mMaterialIndex];
		assert(material);
		Material* newMaterial = new Material();
		newMaterial->shader = mShaderLoader->load(shader);

		loadVertexData(mesh, meshComponent->vertices, meshComponent->indices);

		if(mesh->HasTextureCoords(0))
		{
			loadTextureData(mesh, material, newMaterial);
		}

		if(!loadMaterialProperties(material, newMaterial))
		{
			std::cerr << "SceneImporter: Error loading material properties for: " << scenePath << std::endl;
		}

		meshComponent-> material = newMaterial;
	}
}

void SceneImporter::loadVertexData(aiMesh* srcMesh, std::vector<VertexData>& targetVertices, std::vector<glm::uvec3>& targetIndices)
{
	assert(srcMesh);

	//Set position and normal data
	if (srcMesh->HasTextureCoords(0))
	{
		for (unsigned int j = 0; j < srcMesh->mNumVertices; ++j)
		{
			VertexData vertexData = {
				glm::vec3(srcMesh->mVertices[j].x, srcMesh->mVertices[j].y, srcMesh->mVertices[j].z),
				glm::vec3(srcMesh->mNormals[j].x, srcMesh->mNormals[j].y, srcMesh->mNormals[j].z),
				glm::vec3(srcMesh->mTangents[j].x, srcMesh->mTangents[j].y, srcMesh->mTangents[j].z),
				glm::vec3(srcMesh->mBitangents[j].x, srcMesh->mBitangents[j].y, srcMesh->mBitangents[j].z),
				glm::vec2(srcMesh->mTextureCoords[0][j].x, srcMesh->mTextureCoords[0][j].y)
			};
			targetVertices.push_back(vertexData);
		}
	}
	else
	{
		for (unsigned int j = 0; j < srcMesh->mNumVertices; ++j)
		{
			VertexData vertexData = {
				glm::vec3(srcMesh->mVertices[j].x, srcMesh->mVertices[j].y, srcMesh->mVertices[j].z),
				glm::vec3(srcMesh->mNormals[j].x, srcMesh->mNormals[j].y, srcMesh->mNormals[j].z),
				glm::vec3(srcMesh->mTangents[j].x, srcMesh->mTangents[j].y, srcMesh->mTangents[j].z),
				glm::vec3(srcMesh->mBitangents[j].x, srcMesh->mBitangents[j].y, srcMesh->mBitangents[j].z),
				glm::vec2(0.0f)
			};
			targetVertices.push_back(vertexData);
		}
	}

	//Set indicices data
	for(unsigned int j = 0; j < srcMesh->mNumFaces; ++j)
	{
		targetIndices.push_back(glm::uvec3(srcMesh->mFaces[j].mIndices[0], srcMesh->mFaces[j].mIndices[1], srcMesh->mFaces[j].mIndices[2]));
	}	
}

bool SceneImporter::loadMaterialProperties(aiMaterial* srcMaterial, Material* targetMaterial)
{
	assert(srcMaterial);
	assert(targetMaterial);

	//Load material attributes into material class
	aiColor4D buffer;
	if(AI_SUCCESS != srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE,buffer)) 
	{
		return false;
	}
	targetMaterial->color = glm::vec4(buffer.r, buffer.g, buffer.b, buffer.a);

	if(AI_SUCCESS != srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR,buffer)) 
	{
		return false;
	}
	targetMaterial->specular = glm::vec4(buffer.r, buffer.g, buffer.b, buffer.a);

	if(AI_SUCCESS != srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT,buffer)) 
	{
		return false;
	}
	targetMaterial->ambient = glm::vec4(buffer.r, buffer.g, buffer.b, buffer.a);

	float shininess;
	if(AI_SUCCESS != srcMaterial->Get(AI_MATKEY_SHININESS,shininess)) 
	{
		return false;
	}
	targetMaterial->shininess = shininess;

	return true;
}

void SceneImporter::loadTextureData(aiMesh* srcMesh, aiMaterial* srcMaterial, Material* targetMaterial)
{
	assert(srcMesh);
	assert(srcMaterial);
	assert(targetMaterial);

	//Check for normal or height maps
	if(mScene->mMaterials[srcMesh->mMaterialIndex]->GetTextureCount(aiTextureType_NORMALS) > 0)
	{
		aiString bumpMapPath;
		mScene->mMaterials[srcMesh->mMaterialIndex]->GetTexture(aiTextureType_NORMALS, 0, &bumpMapPath);

		targetMaterial->normalTexture = mTextureLoader->load(bumpMapPath.C_Str());
	}
	else if(mScene->mMaterials[srcMesh->mMaterialIndex]->GetTextureCount(aiTextureType_HEIGHT) > 0)
	{
		aiString bumpMapPath;
		mScene->mMaterials[srcMesh->mMaterialIndex]->GetTexture(aiTextureType_HEIGHT, 0, &bumpMapPath);

		targetMaterial->normalTexture = mTextureLoader->load(bumpMapPath.C_Str());
	}

	//Check for color maps
	if(mScene->mMaterials[srcMesh->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString diffusePath;
		mScene->mMaterials[srcMesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath);
		targetMaterial->texture = mTextureLoader->load(diffusePath.C_Str());
	}
}