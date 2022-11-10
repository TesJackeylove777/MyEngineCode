#pragma once

#if defined(_WIN32)
#include"../EngineMInimal.h"

class FWinMainCommandParameters
{
public:
	FWinMainCommandParameters(HINSTANCE InhInstance, HINSTANCE InprevInstance, PSTR IncmdLine, int InshowCmd);
	
	HINSTANCE HInstance;
	HINSTANCE PrevInstance;
	PSTR CmdLine;
	int ShowCmd;
};

#elif 0

#endif