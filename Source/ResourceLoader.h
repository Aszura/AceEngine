#pragma once
#include <unordered_map>
#include <vector>

namespace utility
{
	template<class T>
	class ResourceLoader
	{
	public:
		inline std::vector<T*>& getUnitializedResources()
		{
			return m_unitializedResources;
		}

		inline std::unordered_map<std::string, T>& getResources()
		{
			return m_resources;
		}

		T* load(std::string resourcePath)
		{
			if (resourcePath[0] == '/')
			{
				resourcePath.erase(resourcePath.begin(), resourcePath.begin() + 1);
			}

			if (m_resources.find(resourcePath) != m_resources.end())
			{
				return &m_resources[resourcePath];
			}
			else
			{
				m_resources[resourcePath] = T();
				m_resources[resourcePath].path = resourcePath;
				m_unitializedResources.push_back(&m_resources[resourcePath]);
				return &m_resources[resourcePath];
			}
		}
	private:
		std::unordered_map<std::string, T>	m_resources;
		std::vector<T*>						m_unitializedResources;
	};
}