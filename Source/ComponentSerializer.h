#pragma once

#include "ApiExports.h"
#include "CustomTypes.h"

#include <string>
#include <unordered_map>
#include <sstream>
#include <functional>

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
			ACE_ENGINE_API ComponentSerializer(EntityWorld& entityWorld, utility::ResourceLoader<rendering::Texture>& textureLoader, utility::ResourceLoader<rendering::Shader>& shaderLoader);

			ACE_ENGINE_API void deserialize(const std::string typeName, std::unordered_map<std::string, std::string>& members);

			ACE_ENGINE_API virtual void deserializeGameSpecific() {};

		protected:
			ACE_ENGINE_API void addDeserializer(const std::string&, std::function<void(EntityId, std::unordered_map<std::string, std::string>&)> deserializationFunction);

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
			std::unordered_map<std::string, std::function<void(EntityId, std::unordered_map<std::string, std::string>&)>> m_deserializers;
		};
	}
}
