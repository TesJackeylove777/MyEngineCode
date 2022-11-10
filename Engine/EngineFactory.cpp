#include"EngineFactory.h"
#if defined(WIN32)
#include"Platform/Windows/WindowsEngine.h"
#endif

FEngineFactory::FEngineFactory()
{

}

CEngine* FEngineFactory::CreateEngine()
{
#if defined(_WIN32)	
	return new CWindowsEngine();
#elif defined(__Linux__)

#endif

}