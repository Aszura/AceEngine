# Ace Engine
The Ace Engine is my personal game engine project. I like working on it in my freetime whenever I don't develop any specific prototypes. Take a look at the Ace Engine 2D branch for the latest changes.

It features a component-based 2D/3D game engine that can be easily extended. Levels are loaded via XML to offer the possibility to implement an independent editor. Currently the renderer is based on the OpenGL API. The components are stored in highly-optimized packed arrays.

# Features
* Extensible component-based game engine
* OpenGL 3D forward renderer
* Normal mapping
* Skyboxes
* Gamma correction
* Terrain generation
* Collisions & Rigid body physics
* Music & Sounds
* Event system
* Resource management
* 3D model loading
* 1st person camera
* XML level loading & deserialization
* Packed arrays

# Libraries
The game engine uses the following libraries:
* SFML: Image loading, context creation, audio and input
* Assimp: 3D model loading
* NVidia PhysX
* OpenGL Math
* Glew

# Building
The project comes with a Visual Studio 2013 solution in the "Source" folder.

# Controls
* Mouse: Look around
* WASD: Move player
* Space: Jump
