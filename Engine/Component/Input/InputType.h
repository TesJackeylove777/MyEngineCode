#pragma once
#include"../../EngineMInimal.h"

enum EPressState
{
	Press,       //����
	Release,     //�ɿ�
};

struct FInputKey
{
	FInputKey();

	string KeyName;
	EPressState PressState;
};