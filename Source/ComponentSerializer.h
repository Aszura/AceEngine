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
			ComponentSerializer(EntityWorld& entityWorld, utility::ResourceLoader<rendering::Texture>& textureLoader, utility::ResourceLoader<rendering::Shader>& shaderLoader);

			void deserialize(const std::string typeName, std::unordered_map<std::string, std::string>& members);
		private:
			template<class T>
			T extractValue(const std::string& text)
			{
				T result;

				m_strStream << text;
				m_strStream >> result;
				m_strStream.clear();
				m_strStream.str("");

				return result;
			}

			template<class T>
			bool loadValue(std::unordered_map<std::string, std::string>& members, const std::string& memberName, T& targetMember)
			{
				if (members.count(memberName) > 0)
				{
					m_strStream << members[memberName];
					m_strStream >> targetMember;
					m_strStream.clear();
					m_strStream.str("");
					return true;
				}

				return false;
			}

			std::stringstream								m_strStream;
			EntityWorld&									m_entityWorld;
			utility::ResourceLoader<rendering::Texture>&	m_textureLoader;
			utility::ResourceLoader<rendering::Shader>&		m_shaderLoader;
		};
	}
}
