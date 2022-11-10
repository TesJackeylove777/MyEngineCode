#pragma once

#include"ViewPort.h"
#include"CoreObject/CoreMinimalObject.h"
#include"../CodeReflection/CodeReflectionMacroTag.h"

struct FInputKey;
class CTransformationComponent;
class CInputComponent;

class CCamera : public CCoreMinimalObject, public FViewPort
{
	CVARIABLE();
	CTransformationComponent* TransformationComponent;

	CVARIABLE();
	CInputComponent* InputComponent;
public:
	CCamera();


	virtual void BeginInit();
	virtual void Tick(float DeltaTime);
	virtual void ExecuteKeyboard(const FInputKey& InputKey);

public:
	FORCEINLINE CInputComponent* GetInputComponent() { return InputComponent; }
	FORCEINLINE CTransformationComponent* GetTransformationComponent() { return TransformationComponent; }
};