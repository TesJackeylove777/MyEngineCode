#pragma once
#include"../../EngineMInimal.h"

enum EPressState
{
	Press,       //╟╢об
	Release,     //ки©╙
};

struct FInputKey
{
	FInputKey();

	string KeyName;
	EPressState PressState;
};