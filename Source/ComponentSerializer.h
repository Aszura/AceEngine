#pragma once

#include <string>
#include <unordered_map>
#include <sstream>

class EntityWorld;
struct Texture;
struct Shader;
template <class T> class ResourceLoader;

class ComponentSerializer
{
public:
	ComponentSerializer(EntityWorld* entityWorld, ResourceLoader<Texture>* textureLoader, ResourceLoader<Shader>* shaderLoader);

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

	std::stringstream			mStrStream;
	EntityWorld*				mEntityWorld;
	ResourceLoader<Texture>*	mTextureLoader;
	ResourceLoader<Shader>*		mShaderLoader;
};