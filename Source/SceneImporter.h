#pragma once
#include "VertexData.h"
#include <vector>

template <class T> class ResourceLoader;
class EntityWorld;
struct MeshComponent;
struct Material;
struct Texture;
struct Shader;
struct aiScene;
struct aiMaterial;
struct aiMesh;

namespace Assimp
{
	class Importer;
};

class SceneImporter
{
public:
	SceneImporter(EntityWorld* entityWorld, ResourceLoader<Texture>* textureLoader, ResourceLoader<Shader>* shaderLoader);
	~SceneImporter();

	void loadScene(unsigned int entityId, const char* scenePath, std::string shader);
private:
	void loadVertexData(aiMesh* srcMesh, std::vector<VertexData>& targetVertices, std::vector<glm::uvec3>& targetIndices);
	void loadTextureData(aiMesh* srcMesh, aiMaterial* srcMaterial, Material* targetMaterial);
	bool loadMaterialProperties(aiMaterial* srcMaterial, Material* targetMaterial);

	EntityWorld*				mEntityWorld;
	ResourceLoader<Texture>*	mTextureLoader;
	ResourceLoader<Shader>*		mShaderLoader;
	Assimp::Importer*			mImporter;
	const aiScene*				mScene;
};