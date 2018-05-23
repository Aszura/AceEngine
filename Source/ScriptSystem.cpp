#include "ScriptSystem.h"
#include "EntityWorld.h"
#include "TransformComponent.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/image.h>

namespace script
{
	ScriptSystem::ScriptSystem(component::EntityWorld& entityWorld) : m_entityWorld(entityWorld)
	{
	}

	ScriptSystem::~ScriptSystem()
	{
		mono_jit_cleanup(m_domain);
	}

	void ScriptSystem::start()
	{
		createComponentScripts();

		mono_set_dirs(".\\mono\\lib", ".\\mono\\etc");
		m_domain = mono_jit_init("AceEngineScript.exe");

		m_assembly = mono_domain_assembly_open(m_domain, "AceEngineScript.exe");
		if (!m_assembly)
		{
			fprintf(stderr, "Error: Couldn't open mono assembly.\n"); 
		}

		m_image = mono_assembly_get_image(m_assembly);

		for (auto& comp : m_entityWorld.getScriptWorld().getArray())
		{
			// Create mono object and call constructor with ptr to C++ object
			comp.monoClass = mono_class_from_name(m_image, "AceEngineScript", comp.className.c_str());
			comp.monoInstance = mono_object_new(m_domain, comp.monoClass);
			MonoMethod* ctor = mono_class_get_method_from_name(comp.monoClass, ".ctor", 1);

			uint64_t ptrUint = reinterpret_cast<uint64_t>(m_entityWorld.getTransformWorld().getFirst(3));
			void* args[1];
			args[0] = &ptrUint;
			mono_runtime_invoke(ctor, comp.monoInstance, args, NULL);

			// Get update method
			comp.updateMethod = mono_class_get_method_from_name(comp.monoClass, "Update", 0);
		}
	}

	void ScriptSystem::update(float deltaTime)  
	{
#ifdef _DEBUG
		MonoObject* exception = nullptr;

		for (auto& comp : m_entityWorld.getScriptWorld().getArray())
		{
			if (comp.updateMethod != nullptr)
			{
				mono_runtime_invoke(comp.updateMethod, comp.monoInstance, nullptr, &exception);

				if (exception)
				{
					std::string excString = mono_string_to_utf8(mono_object_to_string(exception, nullptr));
				}
				else
				{
					exception = nullptr;
				}
			}
		}
#else
		for (auto& comp : m_entityWorld.getScriptWorld().getArray())
		{
			if (comp.updateMethod != nullptr)
			{
				mono_runtime_invoke(comp.updateMethod, comp.monoInstance, NULL, NULL);
			}
		}
#endif
	}

	void ScriptSystem::createComponentScripts()
	{
		for (auto& comp : m_entityWorld.getTransformWorld().getArray())
		{
			auto scriptComp = m_entityWorld.getScriptWorld().add(comp.entityId);
			scriptComp->className = "TransformComponent";
		}
	}
}
