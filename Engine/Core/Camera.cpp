#include "Camera.h"
#include"../Component/InputComponent.h"
#include"../Component/TransformationComponent.h"

CCamera::CCamera()
{
	InputComponent = CreatObject<CInputComponent>(new CInputComponent());
	TransformationComponent = CreatObject<CTransformationComponent>(new CTransformationComponent());
}

void CCamera::BeginInit()
{
	InputComponent->CaptureKeyboardInforDelegate.Bind(this, &CCamera::ExecuteKeyboard);
}

void CCamera::Tick(float DeltaTime)
{
}

void CCamera::ExecuteKeyboard(const FInputKey& InputKey)
{

}
