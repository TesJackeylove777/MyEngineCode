#include"EngineMInimal.h"
#include"EngineFactory.h"
#include"Debug/Log/SimpleLog.h"

//Initialization
int Init(CEngine* InEngine, HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)	//Initialization
{

#if defined(_WIN32)
	FWinMainCommandParameters WinMainParameters(hInstance, prevInstance, cmdLine, showCmd);
#endif	
	int ReturnValue = InEngine->PreInit(
#if defined(_WIN32)
		WinMainParameters
#endif				
	);


	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine Pre initialization error, check and initialize the problem.", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->Init(
#if defined(_WIN32)
		WinMainParameters
#endif
	);
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine initialization error, check and initialize the problem.", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->PostInit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine Post initialization error, check and initialize the problem.", ReturnValue);
		return ReturnValue;
	}
	return ReturnValue;

}

//Rendering
void Tick(CEngine* InEngine)
{
	float DeltaTime = 0.03f;
	InEngine->Tick(DeltaTime);
	Sleep(30);
}

//Exit
int Exit(CEngine* InEngine)
{
	//Exit
	int ReturnValue = InEngine->PreExit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine Pre exit failed.", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->Exit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine exit failed.", ReturnValue);
		return ReturnValue;
	}

	ReturnValue = InEngine->PostExit();
	if (ReturnValue != 0)
	{
		Engine_Log_Error("[%i]Engine Post exit failed.", ReturnValue);
		return ReturnValue;
	}
	return ReturnValue;
}

CEngine* Engine = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)//cmdLine 传递命令  showCmd显示方式
{
	int ReturnValue = 0;
	Engine = FEngineFactory::CreateEngine();
	if (Engine)
	{
		//初始化
		Init(Engine, hInstance, prevInstance, cmdLine, showCmd);
		MSG EngineMsg = { 0 };
		//Rendering
		while (EngineMsg.message != WM_QUIT)
		{
			if (PeekMessage(&EngineMsg,0,0,0, PM_REMOVE))
			{
				TranslateMessage(&EngineMsg);
				DispatchMessage(&EngineMsg);
			}
			else
			{
				Tick(Engine);
			}
		}

		ReturnValue = Exit(Engine);

	}
	else
	{
		ReturnValue = 1;
	}
	Engine_Log("[%i]Engine has exited.", ReturnValue);
	return ReturnValue;
}