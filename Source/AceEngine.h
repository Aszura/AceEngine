#pragma once

#include <Windows.h>

#include "ApiExports.h"

class AceEngine
{
public:
	ACE_ENGINE_API int run(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int iCmdShow);
};