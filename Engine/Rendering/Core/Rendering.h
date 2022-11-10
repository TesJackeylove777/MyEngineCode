#pragma once
#include"../../Core/Engine.h"
#include"../../Core/CoreObject/GuidInterface.h"
#if defined(_WIN32)
#include "../../Platform/Windows/WindowsEngine.h"
#else
#endif

//提供渲染内容的接口
class IRenderingInterface :public IGuidInterface
{
	friend class CWindowsEngine;
public:
	IRenderingInterface();
	virtual ~IRenderingInterface();

	virtual void Init();

	virtual void PreDraw(float DeltaTime);
	virtual void Draw(float DeltaTime);
	virtual void PostDraw(float DeltaTime);


protected:
	ComPtr<ID3D12Resource> ConstructDefaultBuffer(ComPtr<ID3D12Resource>& OutTmpBufffer, const void* InData, UINT64 InDataSize);
protected:
	ComPtr<ID3D12Device> GetD3dDevice();
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList();
	ComPtr<ID3D12CommandAllocator> GetCommandAllocator();

#if defined(_WIN32)
	CWindowsEngine* GetEngine();
#else
	FEngine
#endif

private:
	static vector<IRenderingInterface*> RenderingInterface;
};

class FRenderingResourcesUpdate: public enable_shared_from_this<FRenderingResourcesUpdate>
{
public:
	FRenderingResourcesUpdate();
	~FRenderingResourcesUpdate();

	void Init(ID3D12Device* InDevice, UINT InElementSize, UINT InElementCount);
	
	void Update(int Index, const void *InData);

	UINT GetConstantBufferByteSize(UINT InTypeSize);
	UINT GetConstantBufferByteSize();


	ID3D12Resource* GetBuffer() { return UploadBuffer.Get(); }
private:
	ComPtr<ID3D12Resource> UploadBuffer;
	UINT ElementSize;
	BYTE* Data;
};