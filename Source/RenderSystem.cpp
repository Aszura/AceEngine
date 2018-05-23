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
#include <fstream>
#include <array>

namespace rendering
{
	RenderSystem::RenderSystem(component::EntityWorld& entityWorld, utility::ResourceLoader<Texture>& textureLoader, utility::ResourceLoader<Shader>& shaderLoader, Window& windowData)
		: m_entityWorld(entityWorld)
		, m_textureLoader(textureLoader)
		, m_shaderLoader(shaderLoader)
		, m_windowData(windowData)
	{
	} 


	RenderSystem::~RenderSystem()
	{
		for (auto& texturePair : m_textureLoader.getResources())
		{
			glDeleteTextures(1, &texturePair.second.id);
		}

		for (auto& shaderPair : m_shaderLoader.getResources())
		{
			const Shader& shader = shaderPair.second;
			glDetachShader(shader.id, shader.fragmentShader);
			glDetachShader(shader.id, shader.vertexShader);

			glDeleteShader(shader.fragmentShader);
			glDeleteShader(shader.vertexShader);
			glDeleteProgram(shader.id);
		}

		glDeleteBuffers(1, &m_drawVbo);
		glDeleteBuffers(1, &m_quadVbo);
		glDeleteVertexArrays(1, &m_vao);

		glDeleteBuffers(1, &m_staticUbo);
		glDeleteBuffers(1, &m_dynamicUbo);

		m_windowData.close();
	}

	void RenderSystem::start(unsigned int screenSizeX, unsigned int screenSizeY)
	{
		m_screenSize = glm::uvec2(screenSizeX, screenSizeY);

		createContext();
		initGlew();
		m_windowData.resize(m_screenSize.x, m_screenSize.y);

		// Enable/Disable VSync
		wglSwapIntervalEXT(1);

		{
			using namespace events;
			WindowCreatedData* eventData = new WindowCreatedData();
			eventData->window = &m_windowData;
			EventSystem::fireEvent("WindowCreated", eventData);
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glViewport(0, 0, m_screenSize.x, m_screenSize.y);

		//Set camera viewport
		assert(m_entityWorld.getCameraWorld().getArray().begin() != m_entityWorld.getCameraWorld().getArray().end());
		component::CameraComponent& cameraComp = *m_entityWorld.getCameraWorld().getArray().begin();
		cameraComp.viewportSize = m_screenSize;

		//Generate static uniform buffer object
		glGenBuffers(1, &m_staticUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, m_staticUbo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(StaticBuffer), NULL, GL_DYNAMIC_DRAW);

		//Generate dynamic uniform buffer object
		glGenBuffers(1, &m_dynamicUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, m_dynamicUbo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(DynamicBuffer), NULL, GL_STREAM_DRAW);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//Load data
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
			ss << L"Ace Engine - FPS: " << frames;
			m_windowData.setTitle(ss.str());
			frames = 0;
			frameTimeCounter = 0.0;
		}
	}

	void RenderSystem::createContext()
	{
		PIXELFORMATDESCRIPTOR pfd;
		int format;

		// get the device context (DC)
		*m_windowData.getHDC() = GetDC(*m_windowData.getHWnd()); 

		// set the pixel format for the DC
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 24;
		pfd.iLayerType = PFD_MAIN_PLANE;
		format = ChoosePixelFormat(*m_windowData.getHDC(), &pfd);
		SetPixelFormat(*m_windowData.getHDC(), format, &pfd);

		// create and enable the render context (RC)
		*m_windowData.getHRC() = wglCreateContext(*m_windowData.getHDC());
		wglMakeCurrent(*m_windowData.getHDC(), *m_windowData.getHRC());
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
		for (auto& spriteComp : m_entityWorld.getSpriteWorld().getArray())
		{
			if (!spriteComp.texturePath.empty())
			{
				spriteComp.material = new rendering::Material();
				spriteComp.material->texture = m_textureLoader.load(spriteComp.texturePath);
			}
		}

		for (auto& texture : m_textureLoader.getUnitializedResources())
		{
			texture->image.loadFromFile(texture->path);
			texture->image.flipVertically(); // texture origin -> upper right, openGL origin -> bottom right
			glm::uvec2 size = glm::uvec2(texture->image.getSize().x, texture->image.getSize().y);
			texture->size = size;

			// ==== Upload 2d textures ======
			GLuint id;
			glGenTextures(1, &id);

			// Set internal format and compression
			GLenum internalFormat = GL_RGBA;

			switch (texture->compressionLevel)
			{
			case Texture::CompressionType::None:
				internalFormat = GL_SRGB_ALPHA;
				if (!texture->sRGB)
				{
					internalFormat = GL_RGBA;
				}
				break;
			case Texture::CompressionType::DXT5:
				internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
				if (!texture->sRGB)
				{
					internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				}
				break;
			default:
				assert(false);
				break;
			}

			//Bind normal texture for storing image
			glBindTexture(GL_TEXTURE_2D, id);

			//Load texture from pixels
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->image.getPixelsPtr());

			switch (texture->filteringLevel)
			{
			case Texture::FilteringType::None:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case Texture::FilteringType::Bilinear:
				//Use bilinear interpolation for minification
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				//Use bilinear interpolation for magnification
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case Texture::FilteringType::Trilinear:
				//Generate mipmaps
				glGenerateMipmap(GL_TEXTURE_2D);
				//Use trilinear interpolation for minification
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				//Use bilinear interpolation for magnification
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			default:
				assert(false);
				break;
			}
			
			//Set anisotropic filtering
			texture->anisotropicLevel = std::min(std::max(1, static_cast<int>(texture->anisotropicLevel)), GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT); // clamp to min/max possible values
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, texture->anisotropicLevel);

			texture->id = id;
			texture->isLoaded = true;

			// Delete image data, it's already uploaded to the GPU
			texture->image = sf::Image();
		}

		m_textureLoader.getUnitializedResources().clear();
	}

	void RenderSystem::loadShaders()
	{
		for (auto& spriteComp : m_entityWorld.getSpriteWorld().getArray())
		{
			if (!spriteComp.shaderPath.empty())
			{
				spriteComp.material->shader = m_shaderLoader.load(spriteComp.shaderPath);
			}
		}

		for (auto& shader : m_shaderLoader.getUnitializedResources())
		{
			shader->vertexShader = glCreateShader(GL_VERTEX_SHADER);
			shader->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

			std::string vsFile = shader->path + ".vs";
			std::string psFile = shader->path + ".ps";

			std::string vsText = utility::textFileRead(vsFile);
			std::string psText = utility::textFileRead(psFile);
			std::array<const char*, 1> vsTextStr{ vsText.c_str() };
			std::array<const char*, 1> psTextStr{ psText.c_str() };

			if (vsText.empty() || psText.empty()) {
				std::cerr << "Either vertex or fragment shader file not found." << std::endl;
				return;
			}

			glShaderSource(shader->vertexShader, 1, vsTextStr.data(), 0);
			glShaderSource(shader->fragmentShader, 1, psTextStr.data(), 0);

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
			glBindBufferRange(GL_UNIFORM_BUFFER, staticUboIndex, m_staticUbo, 0, sizeof(StaticBuffer));

			glUniformBlockBinding(shader->id, shader->uniformBlockIndices["DynamicBuffer"], dynamicUboIndex);
			glBindBufferRange(GL_UNIFORM_BUFFER, dynamicUboIndex, m_dynamicUbo, 0, sizeof(DynamicBuffer));
		}

		m_shaderLoader.getUnitializedResources().clear();
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
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_drawVbo);
		glGenBuffers(1, &m_quadVbo);		
		glBindBuffer(GL_ARRAY_BUFFER, m_quadVbo);

		glm::vec2 quad[4];
		quad[0] = glm::vec2(0.0f, 0.0f);
		quad[1] = glm::vec2(1.0f, 0.0f);
		quad[2] = glm::vec2(0.0f, 1.0f);
		quad[3] = glm::vec2(1.0f, 1.0f);

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, quad, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, m_drawVbo);
		unsigned int divisor = 1;

		glVertexAttribPointer(1, 2, GL_UNSIGNED_INT, GL_FALSE, sizeof(SpriteData), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribDivisor(1, divisor);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		for (auto& spriteComp : m_entityWorld.getSpriteWorld().getArray())
		{
			spriteComp.spriteData.size = spriteComp.material->texture->size;
		}
	}

	void RenderSystem::updateStaticBuffer()
	{
		component::CameraComponent* cameraComp = &*m_entityWorld.getCameraWorld().getArray().begin();
		assert(cameraComp);
		component::TransformComponent* cameraTransform = m_entityWorld.getTransformWorld().getFirst(cameraComp->entityId);
		assert(cameraTransform);  

		m_staticBuffer.projectionMatrix = glm::ortho(cameraTransform->position.x, static_cast<float>(m_screenSize.x), cameraTransform->position.y, static_cast<float>(m_screenSize.y), cameraComp->zNear, cameraComp->zFar);
		m_staticBuffer.viewMatrix = glm::lookAt(cameraTransform->position, cameraTransform->position + cameraComp->direction, cameraComp->up);

		glBindBuffer(GL_UNIFORM_BUFFER, m_staticUbo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(StaticBuffer), &m_staticBuffer, GL_DYNAMIC_DRAW); 
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void RenderSystem::update(float deltaTime)
	{
		countFps(deltaTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (m_entityWorld.getCameraWorld().getArray().length() > 0)
		{
			updateStaticBuffer();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			renderMeshes();

			glDisable(GL_BLEND);
			m_windowData.display();
		}
	}

	void RenderSystem::renderMeshes()
	{
		//TODO Create sort

		for (auto& spriteComp : m_entityWorld.getSpriteWorld().getArray())
		{
			component::TransformComponent* transformComp = m_entityWorld.getTransformWorld().getFirst(spriteComp.entityId);
			assert(transformComp);
			Material* material = spriteComp.material;
			assert(material);
			Shader* shader = material->shader;
			assert(shader);

			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transformComp->scale);
			glm::mat4 rotationMatrix = glm::toMat4(transformComp->rotation);
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transformComp->position);

			m_dynamicBuffer.modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
			m_dynamicBuffer.color = material->color;

			glBindBuffer(GL_UNIFORM_BUFFER, m_dynamicUbo);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(DynamicBuffer), &m_dynamicBuffer, GL_STREAM_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glUseProgram(shader->id);

			if (spriteComp.material->texture != nullptr)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material->texture->id);
				glUniform1i(shader->uniformLocations["uColorTex"], 0);
			}

			glBindBuffer(GL_ARRAY_BUFFER, m_drawVbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteData), &spriteComp.spriteData, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(m_vao);
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
			glBindVertexArray(0);

			glUseProgram(0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_drawVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteData), NULL, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}