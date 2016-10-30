#pragma once

#include <string>
#include <unordered_map>
#include <sstream>

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
		class ComponentSerializer
		{
		public:
			ComponentSerializer(EntityWorld* entityWorld, utility::ResourceLoader<rendering::Texture>* textureLoader, utility::ResourceLoader<rendering::Shader>* shaderLoader);

			void deserialize(const std::string typeName, std::unordered_map<std::string, std::string>& members);
		private:
			template<class T>
			T extractValue(const std::string& text)
			{
				T result;

				mStrStream << text;
				mStrStream >> result;
				mStrStream.clear();
				mStrStream.str("");

				return result;
			}

			template<class T>
			bool loadValue(std::unordered_map<std::string, std::string>& members, const std::string& memberName, T& targetMember)
			{
				if (members.count(memberName) > 0)
				{
					mStrStream << members[memberName];
					mStrStream >> targetMember;
					mStrStream.clear();
					mStrStream.str("");
					return true;
				}

				return false;
			}

			std::stringstream								mStrStream;
			EntityWorld*									mEntityWorld;
			utility::ResourceLoader<rendering::Texture>*	mTextureLoader;
			utility::ResourceLoader<rendering::Shader>*		mShaderLoader;
		};
	}
}
