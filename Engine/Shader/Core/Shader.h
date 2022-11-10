#pragma once
#include"../../EngineMInimal.h"


class FShader
{
public:
	LPVOID GetBufferPointer();
	SIZE_T GetBufferSize();
	void BuildShaders(const wstring& InFileName, const string& InEntryFunName, const string& InShaderVersion);
private:
	ComPtr<ID3DBlob> ShaderCode;
};