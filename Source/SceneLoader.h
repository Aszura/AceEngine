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
			SceneLoader(EntityWorld& entityWorld, utility::ResourceLoader<rendering::Texture>& textureLoader, utility::ResourceLoader<rendering::Shader>& shaderLoader);
			~SceneLoader();

			bool loadScene(const std::string& filePath);
		private:
			std::string										m_currentScene;
			EntityWorld&									m_entityWorld;
			utility::ResourceLoader<rendering::Texture>&	m_textureLoader;
			utility::ResourceLoader<rendering::Shader>&		m_shaderLoader;
		};
	}
}
