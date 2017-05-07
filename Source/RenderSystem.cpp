#include "RenderSystem.h"
#include "EntityWorld.h"
#include "ResourceLoader.h"
#include "Texture.h"
#include "Shader.h"
#include "Utilities.h"
#include "InputSystem.h"
#include "EventSystem.h"
#include "Window.h"

#include <sstream>
#include <assert.h>
#include <glm\ext.hpp>

namespace rendering
{
	RenderSystem::RenderSystem(component::EntityWorld* entityWorld, utility::ResourceLoader<Texture>* textureLoader, utility::ResourceLoader<Shader>* shaderLoader, Window* windowData)
		: mEntityWorld(entityWorld)
		, mTextureLoader(textureLoader)
		, mShaderLoader(shaderLoader)
		, mWindowData(windowData)
	{
		assert(entityWorld);
		assert(textureLoader);
		assert(shaderLoader);
		assert(windowData);
	}


	RenderSystem::~RenderSystem()
	{
		for (auto& texturePair : mTextureLoader->getResources())
		{
			glDeleteTextures(1, &texturePair.second.id);
		}

		for (auto& shaderPair : mShaderLoader->getResources())
		{
			const Shader& shader = shaderPair.second;
			glDetachShader(shader.id, shader.fragmentShader);
			glDetachShader(shader.id, shader.vertexShader);

			glDeleteShader(shader.fragmentShader);
			glDeleteShader(shader.vertexShader);
			glDeleteProgram(shader.id);
		}

		glDeleteBuffers(1, &mDrawVbo);
		glDeleteBuffers(1, &mQuadVbo);
		glDeleteVertexArrays(1, &mVao);

		glDeleteBuffers(1, &mStaticUbo);
		glDeleteBuffers(1, &mDynamicUbo);

		mWindowData->close();
	}

	void RenderSystem::start(unsigned int screenSizeX, unsigned int screenSizeY)
	{
		mScreenSize = glm::uvec2(screenSizeX, screenSizeY);

		createContext();
		initGlew();
		mWindowData->resize(mScreenSize.x, mScreenSize.y);

		// Disable VSync
		//wglSwapIntervalEXT(0);

		//sf::ContextSettings settings;
		//settings.antialiasingLevel = 8;
		//settings.depthBits = 24;
		//settings.majorVersion = 3;
		//settings.minorVersion = 3;
		//settings.attributeFlags = settings.Core;

		//mScreenSize = glm::uvec2(screenSizeX, screenSizeY);
		//mWindow = new sf::RenderWindow(sf::VideoMode(mScreenSize.x, mScreenSize.y), "Settle Town", sf::Style::Default, settings);
		//mWindow->setVerticalSyncEnabled(true); 
		//mWindow->setMouseCursorVisible(false);

		//settings = mWindow->getSettings();
		//std::cout << "depth bits:" << settings.depthBits << std::endl;
		//std::cout << "stencil bits:" << settings.stencilBits << std::endl;
		//std::cout << "antialiasing level:" << settings.antialiasingLevel << std::endl;
		//std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;

		{
			using namespace events;
			WindowCreatedData* eventData = new WindowCreatedData();
			eventData->window = mWindowData;
			EventSystem::fireEvent(EventType::WindowCreated, eventData);
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glViewport(0, 0, mScreenSize.x, mScreenSize.y);

		//Set camera viewport
		component::CameraComponent& cameraComp = *mEntityWorld->getCameraWorld().getArray().begin();
		cameraComp.viewportSize = mScreenSize;

		//Generate static uniform buffer object
		glGenBuffers(1, &mStaticUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, mStaticUbo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(StaticBuffer), NULL, GL_DYNAMIC_DRAW);

		//Generate dynamic uniform buffer object
		glGenBuffers(1, &mDynamicUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, mDynamicUbo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(DynamicBuffer), NULL, GL_STREAM_DRAW);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);



		loadTextures();
		loadShaders();
		loadMeshData();
	}

	void RenderSystem::countFps(float deltaTime) const
	{
		//FPS counter
		static int frames = 0;
		static double frameTimeCounter = 0.0;

		frames++;
		if ((frameTimeCounter += deltaTime) > 1)
		{
			std::wostringstream ss;
			ss << L"Settle Town - FPS: " << frames;
			mWindowData->setTitle(ss.str());
			frames = 0;
			frameTimeCounter = 0.0;
		}
	}

	void RenderSystem::createContext()
	{
		PIXELFORMATDESCRIPTOR pfd;
		int format;

		// get the device context (DC)
		*mWindowData->getHDC() = GetDC(*mWindowData->getHWnd()); 

		// set the pixel format for the DC
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 24;
		pfd.iLayerType = PFD_MAIN_PLANE;
		format = ChoosePixelFormat(*mWindowData->getHDC(), &pfd);
		SetPixelFormat(*mWindowData->getHDC(), format, &pfd);

		// create and enable the render context (RC)
		*mWindowData->getHRC() = wglCreateContext(*mWindowData->getHDC());
		wglMakeCurrent(*mWindowData->getHDC(), *mWindowData->getHRC());
	}

	void RenderSystem::initGlew()
	{
		glewExperimental = true; // because version < glew_1.13
		GLenum err = glewInit();

		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
	}

	void RenderSystem::loadTextures()
	{
		for (auto& texture : mTextureLoader->getUnitializedResources())
		{
			texture->image.loadFromFile(texture->path);
			texture->image.flipVertically(); // texture origin -> upper right, openGL origin -> bottom right
			glm::uvec2 size = glm::uvec2(texture->image.getSize().x, texture->image.getSize().y);
			texture->size = size;

			//Upload 2d textures
			GLuint id;
			glGenTextures(1, &id);

			GLenum internalFormat = GL_SRGB_ALPHA;

			if (!texture->sRGB)
			{
				internalFormat = GL_RGBA;
			}

			//Bind normal texture for storing image
			glBindTexture(GL_TEXTURE_2D, id);
			//Load texture from pixels
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->image.getPixelsPtr());
			//Generate mipmaps
			//glGenerateMipmap(GL_TEXTURE_2D);
			//Use trilinear interpolation for minification
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//Use bilinear interpolation for magnification
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//Set anisotropic filtering
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);

			texture->id = id;
			texture->isLoaded = true;
		}

		mTextureLoader->getUnitializedResources().clear();
	}

	void RenderSystem::loadShaders()
	{
		for (auto& shader : mShaderLoader->getUnitializedResources())
		{
			shader->vertexShader = glCreateShader(GL_VERTEX_SHADER);
			shader->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

			std::string vsFile = shader->path + ".vs";
			std::string psFile = shader->path + ".ps";

			const char* vsText = utility::textFileRead(vsFile.c_str());
			const char* psText = utility::textFileRead(psFile.c_str());

			if (vsText == NULL || psText == NULL) {
				std::cerr << "Either vertex or fragment shader file not found." << std::endl;
				return;
			}

			glShaderSource(shader->vertexShader, 1, &vsText, 0);
			glShaderSource(shader->fragmentShader, 1, &psText, 0);

			glCompileShader(shader->vertexShader);
			validateShader(shader->vertexShader, vsFile.c_str());
			glCompileShader(shader->fragmentShader);
			validateShader(shader->fragmentShader, psFile.c_str());

			shader->id = glCreateProgram();
			glAttachShader(shader->id, shader->fragmentShader);
			glAttachShader(shader->id, shader->vertexShader);

			glLinkProgram(shader->id);
			validateShaderProgram(shader->id);

			shader->uniformBlockIndices["StaticBuffer"] = glGetUniformBlockIndex(shader->id, "StaticBuffer");
			shader->uniformBlockIndices["DynamicBuffer"] = glGetUniformBlockIndex(shader->id, "DynamicBuffer");
			shader->uniformLocations["uColorTex"] = glGetUniformLocation(shader->id, "uColorTex");

			glUniformBlockBinding(shader->id, shader->uniformBlockIndices["StaticBuffer"], staticUboIndex);
			glBindBufferRange(GL_UNIFORM_BUFFER, staticUboIndex, mStaticUbo, 0, sizeof(StaticBuffer));

			glUniformBlockBinding(shader->id, shader->uniformBlockIndices["DynamicBuffer"], dynamicUboIndex);
			glBindBufferRange(GL_UNIFORM_BUFFER, dynamicUboIndex, mDynamicUbo, 0, sizeof(DynamicBuffer));
		}

		mShaderLoader->getUnitializedResources().clear();
	}

	void RenderSystem::validateShader(GLuint shader, const char* file) const
	{
		const unsigned int BUFFER_SIZE = 512;
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		GLsizei length = 0;

		glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
		if (length > 0)
		{
			std::cout << "Shader " << shader << " (" << (file ? file : "") << ") compile error: " << buffer << std::endl;
		}
	}

	void RenderSystem::validateShaderProgram(GLuint program) const
	{
		const unsigned int BUFFER_SIZE = 512;
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		GLsizei length = 0;

		memset(buffer, 0, BUFFER_SIZE);
		glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);

		if (length > 0) 
		{
			std::cout << "Program " << program << " link error: " << buffer << std::endl;
		}

		glValidateProgram(program);
		GLint status;
		glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

		if (status == GL_FALSE)
		{
			std::cout << "Error validating shader " << program << std::endl;
		}
	}

	void RenderSystem::loadMeshData()
	{
		glGenVertexArrays(1, &mVao);
		glBindVertexArray(mVao);

		glGenBuffers(1, &mDrawVbo);
		glGenBuffers(1, &mQuadVbo);		
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVbo);

		glm::vec2 quad[4];
		quad[0] = glm::vec2(0.0f, 0.0f);
		quad[1] = glm::vec2(1.0f, 0.0f);
		quad[2] = glm::vec2(0.0f, 1.0f);
		quad[3] = glm::vec2(1.0f, 1.0f);

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, quad, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, mDrawVbo);
		unsigned int divisor = 1;

		glVertexAttribPointer(1, 2, GL_UNSIGNED_INT, GL_FALSE, sizeof(SpriteData), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribDivisor(1, divisor);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		for (auto& meshComp : mEntityWorld->getMeshWorld().getArray())
		{
			meshComp.spriteData.size = meshComp.material->texture->size;
		}
	}

	void RenderSystem::updateStaticBuffer()
	{
		component::CameraComponent* cameraComp = &*mEntityWorld->getCameraWorld().getArray().begin();
		assert(cameraComp);
		component::TransformComponent* cameraTransform = mEntityWorld->getTransformWorld().getFirst(cameraComp->entityId);
		assert(cameraTransform);  

		mStaticBuffer.projectionMatrix = glm::ortho(cameraTransform->position.x, static_cast<float>(mScreenSize.x), cameraTransform->position.y, static_cast<float>(mScreenSize.y), cameraComp->zNear, cameraComp->zFar);

		//-----------------------
		 
		mStaticBuffer.viewMatrix = glm::lookAt(cameraTransform->position, cameraTransform->position + cameraComp->direction, cameraComp->up);

		//-----------------------


		//// Compute inverse rotation q
		//glm::quat rotation = cameraTransform.rotation;
		//rotation.x *= -1.0f;
		//rotation.y *= -1.0f;
		//rotation.z *= -1.0f;
		//mStaticBuffer.viewMatrix = glm::mat4_cast(rotation);

		//// Translate by inverse eyePosition.
		//glm::vec3 v = -cameraTransform.position;
		//glm::mat4 m = mStaticBuffer.viewMatrix;
		//mStaticBuffer.viewMatrix[3] = (m[0] * v[0]) + (m[1] * v[1]) + (m[2] * v[2]) + m[3];

		//glm::vec3 position = cameraTransform.position;
		//glm::vec3 forward = glm::normalize(rotation  * glm::vec3(0.0f, 0.0f, 1.0f));
		//glm::vec3 right = glm::normalize(rotation * glm::vec3(1.0f, 0.0f, 0.0f));
		//glm::vec3 up = glm::cross(right, forward);
		//mStaticBuffer.viewMatrix = glm::lookAt(position, position + forward, up);

		glBindBuffer(GL_UNIFORM_BUFFER, mStaticUbo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(StaticBuffer), &mStaticBuffer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void RenderSystem::update(float deltaTime)
	{
		countFps(deltaTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (mEntityWorld->getCameraWorld().getArray().length() > 0)
		{
			updateStaticBuffer();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			renderMeshes();

			glDisable(GL_BLEND);
			mWindowData->display();
		}
	}

	void RenderSystem::renderMeshes()
	{
		//TODO Create sort

		for (auto& meshComp : mEntityWorld->getMeshWorld().getArray())
		{
			component::TransformComponent* transformComp = mEntityWorld->getTransformWorld().getFirst(meshComp.entityId);
			assert(transformComp);
			Material* material = meshComp.material;
			assert(material);
			Shader* shader = material->shader;
			assert(shader);

			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transformComp->scale);
			glm::mat4 rotationMatrix = glm::toMat4(transformComp->rotation);
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transformComp->position);

			mDynamicBuffer.modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
			mDynamicBuffer.color = material->color;

			glBindBuffer(GL_UNIFORM_BUFFER, mDynamicUbo);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(DynamicBuffer), &mDynamicBuffer, GL_STREAM_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glUseProgram(shader->id);

			if (meshComp.material->texture != nullptr)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material->texture->id);
				glUniform1i(shader->uniformLocations["uColorTex"], 0);
			}

			glBindBuffer(GL_ARRAY_BUFFER, mDrawVbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteData), &meshComp.spriteData, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(mVao);
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
			glBindVertexArray(0);

			glUseProgram(0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, mDrawVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteData), NULL, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}