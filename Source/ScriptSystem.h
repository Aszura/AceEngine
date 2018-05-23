#pragma once

struct _MonoDomain;
struct _MonoAssembly;
struct _MonoImage;

using MonoDomain = _MonoDomain;
using MonoAssembly = _MonoAssembly;
using MonoImage = _MonoImage;

namespace component
{
	class EntityWorld;
}

namespace script
{
	class ScriptSystem
	{
	public:
		explicit ScriptSystem(component::EntityWorld& entityWorld);
		~ScriptSystem();

		void start();
		void update(float deltaTime);
	private:
		void createComponentScripts();

		component::EntityWorld& m_entityWorld;

		MonoDomain* m_domain;
		MonoAssembly* m_assembly;
		MonoImage* m_image;
	};
}
