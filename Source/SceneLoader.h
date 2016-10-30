#pragma once

#include <string>

namespace rendering
{
	struct Texture;
	struct Shader;
}

namespace utility
{
	template <class T> class ResourceLoader;
}

namespace component
{
	class EntityWorld;

	namespace serialization
	{
		class SceneLoader
		{
		public:
			SceneLoader(EntityWorld* entityWorld, utility::ResourceLoader<rendering::Texture>* textureLoader, utility::ResourceLoader<rendering::Shader>* shaderLoader);
			~SceneLoader();

			bool loadScene(const std::string& filePath);
		private:
			std::string										mCurrentScene;
			EntityWorld*									mEntityWorld;
			utility::ResourceLoader<rendering::Texture>*	mTextureLoader;
			utility::ResourceLoader<rendering::Shader>*		mShaderLoader;
		};
	}
}
