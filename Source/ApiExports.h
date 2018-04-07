#pragma once

#ifdef ACE_ENGINE_EXPORTS
#define ACE_ENGINE_API __declspec(dllexport)
#else
#define ACE_ENGINE_API /*__declspec(dllimport)*/
#endif