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
			return mUnitializedResources;
		}

		inline std::unordered_map<std::string, T>& getResources()
		{
			return mResources;
		}

		T* load(std::string resourcePath)
		{
			if (resourcePath[0] == '/')
			{
				resourcePath.erase(resourcePath.begin(), resourcePath.begin() + 1);
			}

			if (mResources.find(resourcePath) != mResources.end())
			{
				return &mResources[resourcePath];
			}
			else
			{
				mResources[resourcePath] = T();
				mResources[resourcePath].path = resourcePath;
				mUnitializedResources.push_back(&mResources[resourcePath]);
				return &mResources[resourcePath];
			}
		}
	private:
		std::unordered_map<std::string, T>	mResources;
		std::vector<T*>						mUnitializedResources;
	};
}