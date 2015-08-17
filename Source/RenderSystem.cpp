#include "RenderSystem.h"
#include "EntityWorld.h"
#include "ResourceLoader.h"
#include "Texture.h"
#include "Shader.h"
#include "Utilities.h"
#include "InputSystem.h"
#include "EventSystem.h"

#include <SFML\Graphics.hpp>
#include <SFML\Window.hpp>
#include <sstream>
#include <assert.h>
#include <glm\ext.hpp>

RenderSystem::RenderSystem(EntityWorld* entityWorld, ResourceLoader<Texture>* textureLoader, ResourceLoader<Shader>* shaderLoader)
	: mEntityWorld(entityWorld)
	, mTextureLoader(textureLoader)
	, mShaderLoader(shaderLoader)
{
	assert(entityWorld);
	assert(textureLoader);
	assert(shaderLoader);
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

	for (auto& meshComp : mEntityWorld->getMeshWorld().getArray())
	{
		glDeleteBuffers(1, &meshComp.vbo);
		glDeleteBuffers(1, &meshComp.ibo);
		glDeleteVertexArrays(1, &meshComp.vao);
	}

	glDeleteBuffers(1, &mStaticUbo);
	glDeleteBuffers(1, &mDynamicUbo);
	glDeleteBuffers(1, &mLightUbo);

	glDeleteBuffers(1, &mSkyboxData.vbo);
	glDeleteBuffers(1, &mSkyboxData.ibo);
	glDeleteVertexArrays(1, &mSkyboxData.vao);

	mWindow->close();
	delete mWindow;
}

void RenderSystem::start(unsigned int screenSizeX, unsigned int screenSizeY)
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	settings.depthBits = 24;

	mScreenSize = glm::uvec2(screenSizeX, screenSizeY);
	mWindow = new sf::RenderWindow(sf::VideoMode(mScreenSize.x, mScreenSize.y), "Settle Town", sf::Style::Default, settings);
	mWindow->setVerticalSyncEnabled(true);
	mWindow->setMouseCursorVisible(false);

	settings = mWindow->getSettings();
	std::cout << "depth bits:" << settings.depthBits << std::endl;
	std::cout << "stencil bits:" << settings.stencilBits << std::endl;
	std::cout << "antialiasing level:" << settings.antialiasingLevel << std::endl;
	std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;

	{
		using namespace events;
		WindowCreatedData* eventData = new WindowCreatedData();
		eventData->window = mWindow;
		EventSystem::fireEvent(EventType::WindowCreated, eventData);
	}

	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glViewport(0, 0, mScreenSize.x, mScreenSize.y);

	//Set camera viewport
	CameraComponent& cameraComp = *mEntityWorld->getCameraWorld().getArray().begin();
	cameraComp.viewportSize = mScreenSize;

	//Generate static uniform buffer object
	glGenBuffers(1, &mStaticUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, mStaticUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(StaticBuffer), NULL, GL_DYNAMIC_DRAW);

	//Generate dynamic uniform buffer object
	glGenBuffers(1, &mDynamicUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, mDynamicUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(DynamicBuffer), NULL, GL_STREAM_DRAW);

	//Generate light uniform buffer object
	glGenBuffers(1, &mLightUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, mLightUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBuffer), NULL, GL_STREAM_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	mSkyboxData.shader = mShaderLoader->load("Skybox");

	loadTextures(); 
	loadShaders();
	loadMeshData();
	loadTerrainData();
	loadSkyboxData();
}

void RenderSystem::countFps(float deltaTime) const
{
	//FPS counter
	static int frames = 0;
	static double frameTimeCounter = 0.0;

	frames++;
	if ((frameTimeCounter += deltaTime) > 1)
	{
		std::ostringstream ss;
		ss << "Settle Town - FPS: " << frames;
		mWindow->setTitle(ss.str());
		frames = 0;
		frameTimeCounter = 0.0;
	}
}

void RenderSystem::handleEvents(bool& running)
{
	// Handle SFML events
	sf::Event event;
	while (mWindow->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			// end the program
			running = false;
		}
	}
}

void RenderSystem::loadTextures()
{
	for (auto& texture : mTextureLoader->getUnitializedResources())
	{
		texture->image.loadFromFile(texture->path);
		glm::uvec2 size = glm::uvec2(texture->image.getSize().x, texture->image.getSize().y);
		texture->size = size;

		//Upload 2d textures
		if (texture->type == TextureType::Texture2D)
		{
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
			glGenerateMipmap(GL_TEXTURE_2D);
			//Use trilinear interpolation for minification
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			//Use bilinear interpolation for magnification
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//Set anisotropic filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);

			texture->id = id;
			texture->isLoaded = true;
		}
	}

	mTextureLoader->getUnitializedResources().clear();

	//Upload cubemap for skybox
	for (auto& skyboxComp : mEntityWorld->getSkyboxWorld().getArray())
	{
		glGenTextures(1, &skyboxComp.cubemapId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxComp.cubemapId);

		for (size_t i = 0; i < 6; i++)
		{
			//Load texture from pixels
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, skyboxComp.textures[i]->size.x, skyboxComp.textures[i]->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, skyboxComp.textures[i]->image.getPixelsPtr());
			//Use trilinear interpolation for minification
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//Use bilinear interpolation for magnification
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//Set wrap mode
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			skyboxComp.textures[i]->isLoaded = true;
		}
	}
}

void RenderSystem::loadShaders()
{
	for (auto& shader : mShaderLoader->getUnitializedResources())
	{
		shader->vertexShader = glCreateShader(GL_VERTEX_SHADER);
		shader->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		std::string vsFile = shader->path + ".vs";
		std::string psFile = shader->path + ".ps";

		const char* vsText = utilities::textFileRead(vsFile.c_str());
		const char* psText = utilities::textFileRead(psFile.c_str());

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
		shader->uniformBlockIndices["LightBuffer"] = glGetUniformBlockIndex(shader->id, "LightBuffer");
		shader->uniformLocations["uColorTex"] = glGetUniformLocation(shader->id, "uColorTex");
		shader->uniformLocations["uNormalTex"] = glGetUniformLocation(shader->id, "uNormalTex");

		glUniformBlockBinding(shader->id, shader->uniformBlockIndices["StaticBuffer"], staticUboIndex);
		glBindBufferRange(GL_UNIFORM_BUFFER, staticUboIndex, mStaticUbo, 0, sizeof(StaticBuffer));

		glUniformBlockBinding(shader->id, shader->uniformBlockIndices["DynamicBuffer"], dynamicUboIndex);
		glBindBufferRange(GL_UNIFORM_BUFFER, dynamicUboIndex, mDynamicUbo, 0, sizeof(DynamicBuffer));

		glUniformBlockBinding(shader->id, shader->uniformBlockIndices["LightBuffer"], lightUboIndex);
		glBindBufferRange(GL_UNIFORM_BUFFER, lightUboIndex, mLightUbo, 0, sizeof(LightBuffer));
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
	for (auto& meshComp : mEntityWorld->getMeshWorld().getArray())
	{
		glGenVertexArrays(1, &meshComp.vao);
		glBindVertexArray(meshComp.vao);

		glGenBuffers(1, &meshComp.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshComp.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * meshComp.vertices.size(), &meshComp.vertices[0], GL_STATIC_DRAW);

		//Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
		glEnableVertexAttribArray(0);

		//Normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void*>(sizeof(glm::vec3)));
		glEnableVertexAttribArray(1);

		//Tangents
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void*>(sizeof(glm::vec3) * 2));
		glEnableVertexAttribArray(2);

		//Bitangents
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void*>(sizeof(glm::vec3) * 3));
		glEnableVertexAttribArray(3);

		//Uvs
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void*>(sizeof(glm::vec3) * 4));
		glEnableVertexAttribArray(4);

		glGenBuffers(1, &meshComp.ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshComp.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * meshComp.indices.size(), &meshComp.indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void RenderSystem::loadTerrainData()
{
	for (auto& terrainComp : mEntityWorld->getTerrainWorld().getArray())
	{
		//Generate VAO
		glGenVertexArrays(1, &terrainComp.vao);
		//Bind VAO
		glBindVertexArray(terrainComp.vao);

		//Generate VBO for vertices, normals and texture coordinates
		glGenBuffers(1, &terrainComp.vbo);
		//Bind VBO in order to use
		glBindBuffer(GL_ARRAY_BUFFER, terrainComp.vbo);

		//Generate buffer data set
		glBufferData(GL_ARRAY_BUFFER, terrainComp.terrainData.size() * sizeof(TerrainVertexData), &terrainComp.terrainData[0], GL_STATIC_DRAW);

		//Positions
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertexData), 0);
		glEnableVertexAttribArray(0);

		//Normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertexData), reinterpret_cast<void*>(sizeof(glm::vec2)));
		glEnableVertexAttribArray(1);

		//Uvs
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertexData), reinterpret_cast<void*>(sizeof(glm::vec2) + sizeof(glm::vec3)));
		glEnableVertexAttribArray(2);

		//Generate VBO for indices
		glGenBuffers(1, &terrainComp.ibo);

		//Bind VBO for indices in order to use
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainComp.ibo);

		//Generate buffer data set for indices
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainComp.terrainIndices.size() * sizeof(unsigned int), &terrainComp.terrainIndices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void RenderSystem::loadSkyboxData()
{
	const GLfloat cubeVertices[] = {
		// front
		-1.0, -1.0, 1.0,
		1.0, -1.0, 1.0,
		1.0, 1.0, 1.0,
		-1.0, 1.0, 1.0,
		// back
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0, 1.0, -1.0,
		-1.0, 1.0, -1.0,
	};

	const GLuint cubeIndices[] = {
			// front
			0, 1, 2,
			2, 3, 0,
			// top
			3, 2, 6,
			6, 7, 3,
			// back
			7, 6, 5,
			5, 4, 7,
			// bottom
			4, 5, 1,
			1, 0, 4,
			// left
			4, 0, 3,
			3, 7, 4,
			// right
			1, 5, 6,
			6, 2, 1,
		};

	glGenVertexArrays(1, &mSkyboxData.vao);
	glBindVertexArray(mSkyboxData.vao);

	glGenBuffers(1, &mSkyboxData.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mSkyboxData.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &mSkyboxData.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mSkyboxData.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderSystem::updateStaticBuffer()
{
	CameraComponent* cameraComp = &*mEntityWorld->getCameraWorld().getArray().begin();
	assert(cameraComp);
	TransformComponent* cameraTransform = mEntityWorld->getTransformWorld().getFirst(cameraComp->entityId);
	assert(cameraTransform);

	mStaticBuffer.projectionMatrix = glm::perspective(glm::radians(cameraComp->fov), static_cast<float>(mScreenSize.x) / static_cast<float>(mScreenSize.y), cameraComp->zNear, cameraComp->zFar);

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

void RenderSystem::update(float deltaTime, bool& running)
{
	countFps(deltaTime);
	handleEvents(running);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (mEntityWorld->getCameraWorld().getArray().length() > 0)
	{
		updateStaticBuffer();
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (auto& lightComp : mEntityWorld->getLightWorld().getArray())
		{
			TransformComponent* lightTransform = mEntityWorld->getTransformWorld().getFirst(lightComp.entityId);
			assert(lightTransform);

			mLightBuffer.color = lightComp.color;
			mLightBuffer.ambient = lightComp.ambient;
			mLightBuffer.position = lightTransform->position;
			mLightBuffer.intensity = lightComp.intensity;

			glBindBuffer(GL_UNIFORM_BUFFER, mLightUbo);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBuffer), &mLightBuffer, GL_STREAM_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			renderTerrain();
			renderMeshes();
			renderSkybox();
		}

		glDisable(GL_BLEND);
		mWindow->display();
	}
}

void RenderSystem::renderSkybox()
{
	CameraComponent* cameraComp = &*mEntityWorld->getCameraWorld().getArray().begin();
	assert(cameraComp);
	TransformComponent* cameraTransform = mEntityWorld->getTransformWorld().getFirst(cameraComp->entityId);
	assert(cameraTransform);

	glCullFace(GL_FRONT);

	glUseProgram(mSkyboxData.shader->id);
	glBindVertexArray(mSkyboxData.vao);

	for (auto& skyboxComp : mEntityWorld->getSkyboxWorld().getArray())
	{
		mDynamicBuffer.modelMatrix = glm::translate(glm::mat4(1.0f), cameraTransform->position);

		glBindBuffer(GL_UNIFORM_BUFFER, mDynamicUbo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(DynamicBuffer), &mDynamicBuffer, GL_STREAM_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxComp.cubemapId);
		glUniform1i(mSkyboxData.shader->uniformLocations["uColorTex"], 0);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
	glUseProgram(0);

	glCullFace(GL_BACK);
}

void RenderSystem::renderTerrain()
{
	glEnable(GL_PRIMITIVE_RESTART);

	for (auto& terrainComp : mEntityWorld->getTerrainWorld().getArray())
	{
		TransformComponent* transformComp = mEntityWorld->getTransformWorld().getFirst(terrainComp.entityId);
		assert(transformComp);
		Material* material = terrainComp.material;
		assert(material);
		Shader* shader = material->shader;
		assert(shader);

		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transformComp->scale);
		glm::mat4 rotationMatrix = glm::toMat4(transformComp->rotation);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transformComp->position);

		mDynamicBuffer.modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
		mDynamicBuffer.color = material->color;
		mDynamicBuffer.specular = material->specular;
		mDynamicBuffer.ambient = material->ambient;
		mDynamicBuffer.shininess = material->shininess;

		glBindBuffer(GL_UNIFORM_BUFFER, mDynamicUbo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(DynamicBuffer), &mDynamicBuffer, GL_STREAM_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glUseProgram(shader->id);
		glBindVertexArray(terrainComp.vao);

		glUniform1ui(glGetUniformLocation(shader->id, "uTiling"), terrainComp.tiling);
		glUniform1f(glGetUniformLocation(shader->id, "uMaxTerrainHeight"), terrainComp.maxHeight);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->texture->id);
		glUniform1i(shader->uniformLocations["uColorTex"], 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, terrainComp.heightTexture->id);
		glUniform1i(glGetUniformLocation(shader->id, "uHeightMap"), 1);

		glPrimitiveRestartIndex(terrainComp.heightTexture->size.x * terrainComp.heightTexture->size.y);
		glDrawElements(GL_TRIANGLE_STRIP, terrainComp.indicesNum, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	glDisable(GL_PRIMITIVE_RESTART);
}

void RenderSystem::renderMeshes()
{
	for (auto& meshComp : mEntityWorld->getMeshWorld().getArray())
	{
		TransformComponent* transformComp = mEntityWorld->getTransformWorld().getFirst(meshComp.entityId);
		assert(transformComp);
		Material* material = meshComp.material;
		assert(material);
		Shader* shader = material->shader;
		assert(shader);

		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transformComp->scale); 
		glm::mat4 rotationMatrix = glm::toMat4(transformComp->rotation); 
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transformComp->position);

		mDynamicBuffer.modelMatrix = translationMatrix * rotationMatrix * scaleMatrix; 
		mDynamicBuffer.normalMatrix = glm::inverseTranspose(mStaticBuffer.viewMatrix * mDynamicBuffer.modelMatrix); 
		mDynamicBuffer.color = material->color;
		mDynamicBuffer.specular = material->specular;
		mDynamicBuffer.ambient = material->ambient;
		mDynamicBuffer.shininess = material->shininess;

		glBindBuffer(GL_UNIFORM_BUFFER, mDynamicUbo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(DynamicBuffer), &mDynamicBuffer, GL_STREAM_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glUseProgram(shader->id);
		glBindVertexArray(meshComp.vao);

		if (meshComp.material->texture != nullptr)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material->texture->id);
			glUniform1i(shader->uniformLocations["uColorTex"], 0);

			if (meshComp.material->normalTexture != nullptr)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, material->normalTexture->id);
				glUniform1i(shader->uniformLocations["uNormalTex"], 1);
			}
		}

		glDrawElements(GL_TRIANGLES, meshComp.indices.size() * 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}