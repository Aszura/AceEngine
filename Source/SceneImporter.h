#pragma once
#include "VertexData.h"
#include <vector>

namespace utility
{
	template <class T> class ResourceLoader;
}

namespace component
{
	class EntityWorld;
	struct MeshComponent;
}

namespace rendering
{
	struct Material;
	struct Texture;
	struct Shader;
}

struct aiScene;
struct aiMaterial;
struct aiMesh;

namespace Assimp
{
	class Importer;
};

namespace rendering
{
	class SceneImporter
	{
	public:
		SceneImporter(component::EntityWorld* entityWorld, utility::ResourceLoader<Texture>* textureLoader, utility::ResourceLoader<Shader>* shaderLoader);
		~SceneImporter();

		void loadScene(unsigned int entityId, const char* scenePath, std::string shader);
	private:
		void loadVertexData(aiMesh* srcMesh, std::vector<rendering::VertexData>& targetVertices, std::vector<glm::uvec3>& targetIndices);
		void loadTextureData(aiMesh* srcMesh, aiMaterial* srcMaterial, Material* targetMaterial);
		bool loadMaterialProperties(aiMaterial* srcMaterial, Material* targetMaterial);

		component::EntityWorld*				mEntityWorld;
		utility::ResourceLoader<Texture>*	mTextureLoader;
		utility::ResourceLoader<Shader>*		mShaderLoader;
		Assimp::Importer*			mImporter;
		const aiScene*				mScene;
	};
}