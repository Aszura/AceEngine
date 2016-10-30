#pragma once
#include "Shader.h"

#include <GL\glew.h>
#include <GL\wglew.h>
#include <gl\gl.h>
#include <glm\glm.hpp>

namespace component
{
	class EntityWorld;
}

namespace utility
{
	template <class T> class ResourceLoader;
}

namespace rendering
{
	class Window;
	struct Texture;

	class RenderSystem
	{
	public:
		RenderSystem(component::EntityWorld* entityWorld, utility::ResourceLoader<Texture>* textureLoader, utility::ResourceLoader<Shader>* shaderLoader, Window* windowData);
		~RenderSystem();

		void start(unsigned int screenSizeX, unsigned int screenSizeY);
		void update(float deltaTime);

	private:
		struct StaticBuffer
		{
			glm::mat4	projectionMatrix;
			glm::mat4	viewMatrix;
		};

		struct DynamicBuffer
		{
			glm::mat4	modelMatrix;
			glm::mat4	normalMatrix;
			glm::vec4	color;
			glm::vec4	specular;
			glm::vec4	ambient;
			float		shininess;
		};

		struct LightBuffer
		{
			glm::vec4	color;
			glm::vec4	ambient;
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

		struct RenderKey
		{
			unsigned short shaderId;
			unsigned short textureId;
		};

		void createContext();
		void initGlew();
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
		component::EntityWorld*					mEntityWorld;
		Window*							mWindowData;
		//sf::RenderWindow*				mWindow;
		utility::ResourceLoader<Texture>*		mTextureLoader;
		utility::ResourceLoader<Shader>*			mShaderLoader;
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

}