#pragma once

#include <functional>

namespace input
{
	struct InputCallback
	{
		std::function< short(int) > keyCallback;
		std::function< void(long&, long&) > mousePosCallback;
	};
}