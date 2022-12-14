#pragma once
#include "Core/Component.h"
#include"Input/InputType.h"

DEFINITION_SIMPLE_SINGLE_DELEGATE(FCaptureKeyboardInforDelegate, void, const FInputKey&);

class CInputComponent : public CComponent
{
public:
	CVARIABLE()
	FCaptureKeyboardInforDelegate CaptureKeyboardInforDelegate;

public:
	virtual void BeginInit();
	virtual void Tick(float DeltaTime);
};