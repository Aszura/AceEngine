#pragma once
#include "CustomTypes.h"
#include <string>

struct _MonoClass;
struct _MonoObject;
struct _MonoMethod;

using MonoClass = _MonoClass;
using MonoObject = _MonoObject;
using MonoMethod = _MonoMethod;

namespace component
{
	struct ScriptComponent
	{
		EntityId	entityId;
		ElementId	componentId;

		std::string className;
		MonoClass* monoClass{ nullptr };
		MonoObject* monoInstance{ nullptr };
		MonoMethod* updateMethod{ nullptr };
	};
}
