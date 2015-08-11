#pragma once
#include "Shader.h"

#include <GL\glew.h>
#include <SFML\OpenGL.hpp>
#include <glm\glm.hpp>

class EntityWorld;
struct Texture;
template <class T> class ResourceLoader;

namespace sf
{
	class RenderWindow;
}

class RenderSystem
{
public:
	RenderSystem(EntityWorld* entityWorld, ResourceLoader<Texture>* textureLoader, ResourceLoader<Shader>* shaderLoader);
	~RenderSystem();

	void start(unsigned int screenSizeX, unsigned int screenSizeY);
	void update(float deltaTime, bool& running);

private:
	struct StaticBuffer
	{
		glm::mat4	projectionMatrix;
		glm::mat4	viewMatrix;
	};

	struct DynamicBuffer
	{
		glm::mat4	modelMatrix;
		glm::vec4	color;
		glm::vec4	specular;
		glm::vec4	ambient;
		float		shininess;
	};

	struct LightBuffer
	{
		glm::vec4	color;
		glm::vec3	position;
		float		intensity;
	};

	struct SkyboxData
	{
		GLuint		vbo;
		GLuint		ibo;
		GLuint		vao;
		Shader*		shader;
	};

	void handleEvents(bool& running);
	void countFps(float deltaTime) const;
	void loadTextures();
	void loadShaders();
	void validateShader(GLuint shader, const char* file = 0) const;
	void validateShaderProgram(GLuint program) const;
	void loadMeshData();
	void loadTerrainData();
	void loadSkyboxData();
	void updateStaticBuffer();
	void renderSkybox();
	void renderTerrain();
	void renderMeshes();

	glm::uvec2						mScreenSize;
	EntityWorld*					mEntityWorld;
	sf::RenderWindow*				mWindow;
	ResourceLoader<Texture>*		mTextureLoader;
	ResourceLoader<Shader>*			mShaderLoader;
	SkyboxData						mSkyboxData;
	StaticBuffer					mStaticBuffer;
	DynamicBuffer					mDynamicBuffer;
	LightBuffer						mLightBuffer;
	GLuint							mStaticUbo;
	GLuint							mDynamicUbo;
	GLuint							mLightUbo;

	static const int			staticUboIndex = 1;
	static const int			dynamicUboIndex = 2;
	static const int			lightUboIndex = 3;
};

