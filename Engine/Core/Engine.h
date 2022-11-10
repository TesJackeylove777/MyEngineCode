#pragma once
#include"CoreObject/CoreMinimalObject.h"

#if (_WIN32)
#include"WinMainCommandParameters.h"
#endif

class CEngine :public CCoreMinimalObject
{
public:
	CEngine();
	virtual int PreInit(
#if defined(_WIN32)	
		FWinMainCommandParameters InPrameters
#endif
	) = 0;
	virtual int Init(
#if defined(_WIN32)	
		FWinMainCommandParameters InPrameters
#endif
	) = 0;
	virtual int PostInit() = 0;

	virtual void Tick(float DeltaTime) {};

	virtual int PreExit() = 0;
	virtual int Exit() = 0;
	virtual int PostExit() = 0;
};