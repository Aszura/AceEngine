#pragma once

#include <string>

class EntityWorld;
struct Texture;
struct Shader;
template <class T> class ResourceLoader;

class SceneLoader
{
public:
	SceneLoader(EntityWorld* entityWorld, ResourceLoader<Texture>* textureLoader, ResourceLoader<Shader>* shaderLoader);
	~SceneLoader();

	bool loadScene(const std::string& filePath);
private:
	std::string					mCurrentScene;
	EntityWorld*				mEntityWorld;
	ResourceLoader<Texture>*	mTextureLoader;
	ResourceLoader<Shader>*		mShaderLoader;
};

