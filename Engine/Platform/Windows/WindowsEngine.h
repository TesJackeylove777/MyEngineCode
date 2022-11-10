#pragma once
#if defined(_WIN32)
#include"../../Core/Engine.h"

class CWindowsEngine : public CEngine
{
	friend class IRenderingInterface;
public:
	CWindowsEngine();

	~CWindowsEngine();
	virtual int PreInit(FWinMainCommandParameters InPrameters);

	virtual int Init(FWinMainCommandParameters InPrameters);
	virtual int PostInit();


	virtual void Tick(float DeltaTime);

	virtual int PreExit();
	virtual int Exit();
	virtual int PostExit();



public:
	ID3D12Resource* GetCurrentSwapBuff() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentSwapBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilView()const;
public:
	DXGI_FORMAT GetBackBufferFormat() const { return BackBufferFormat; }
	DXGI_FORMAT GetDepthStencilFormat() const { return DepthStencilFormat; }
	UINT GetDXGISampleCount() const;
	UINT GetDXGISampleQuality() const;
protected:
	void WaitGPUCommandQueueComplete();

private:
	bool InitWindows(FWinMainCommandParameters InPrameters);
	bool InitDirect3D();
	void PostInitDirect3D();
protected:
	UINT64 CurrentFenceIndex;
	int CurrentSwapBuffIndex;


	ComPtr<IDXGIFactory4> DXGIFactory; //Creat DirectX DXGI object
	ComPtr<ID3D12Device> D3dDevice; //Creat CommandAllocator, CommandList, CommandQueue, Fence, Resources, Heap, 
	ComPtr<ID3D12Fence> Fence; //Make CPU synchronize with multiple GPU objects

	ComPtr<ID3D12CommandQueue> CommandQueue; //Queue
	ComPtr<ID3D12CommandAllocator> CommandAllocator; //Storage
	ComPtr<ID3D12GraphicsCommandList> GraphicsCommandList; //CommandList

	ComPtr<IDXGISwapChain> SwapChain;

	//描述符对象和堆
	ComPtr<ID3D12DescriptorHeap> RTVHeap;
	ComPtr<ID3D12DescriptorHeap> DSVHeap;

	vector<ComPtr<ID3D12Resource>> SwapChainBuffer;
	ComPtr<ID3D12Resource> DepthStencilBuffer;
	
	//与屏幕视口有关
	D3D12_VIEWPORT ViewPortInfo;
	D3D12_RECT ViewPortRect;
protected:
	HWND MainWindowsHandle; //主windows句柄
	UINT M4XQualityLevels;
	bool bMSAA4XEnabled;
	DXGI_FORMAT BackBufferFormat;
	DXGI_FORMAT DepthStencilFormat;
	UINT RTVDescriptorSize;
};

#endif
