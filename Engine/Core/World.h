#pragma once
#include"ViewPort.h"
#include"CoreObject/CoreMinimalObject.h"
#include"../CodeReflection/CodeReflectionMacroTag.h"

struct FInputKey;
class CTransformationComponent;
class CInputComponent;
class CCamera;

class CWorld : public CCoreMinimalObject
{
public:
	CWorld();
	CVARIABLE();
	CCamera* Camera;
};