#include"WindowsEngine.h"
#include"../../Debug/EngineDebug.h"
#include"../../Config/EngineRenderConfig.h"
#include"../../Rendering/Core/Rendering.h"
#if defined(_WIN32)
#include"WindowsMessageProcessing.h"
#include"../../Mesh/BoxMesh.h"
#include"../../Mesh/SphereMesh.h"
#include"../../Mesh/CylinderMesh.h"
#include"../../Mesh/ConeMesh.h"
#include"../../Mesh/PlaneMesh.h"
#include"../../Mesh/CustomMesh.h"
#include"../../Core/CoreObject/CoreMinimalObject.h"
#include"../../Core/World.h"


CWindowsEngine::CWindowsEngine()
	:CurrentFenceIndex(0)
	,M4XQualityLevels(0)
	, bMSAA4XEnabled(false)
	, BackBufferFormat(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM)
	, DepthStencilFormat(DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT)
	, CurrentSwapBuffIndex(0)
{
	for (int i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
	{
		SwapChainBuffer.push_back(ComPtr<ID3D12Resource>());
	}
	bTick = false;
}

CWindowsEngine::~CWindowsEngine()
{
}

int CWindowsEngine::PreInit(FWinMainCommandParameters InPrameters)
{
	//Logging system initialization
	const char LogPath[] = "../log";
	init_log_system(LogPath);
	Engine_Log("Log Init.");

	//Processing Command


	Engine_Log("Engine pre initialization complete.");
	return 0;
}

int CWindowsEngine::Init(FWinMainCommandParameters InPrameters)
{

	InitWindows(InPrameters);

	InitDirect3D();

	PostInitDirect3D();

	CWorld *World = CreatObject<CWorld>(new CWorld());

	Engine_Log("Engine initialization complete.");
	return 0;
}

int CWindowsEngine::PostInit()
{
	Engine_Log("Engine post initialization complete.");
	ANALYSIS_HRESULT(GraphicsCommandList->Reset(CommandAllocator.Get(), NULL));
	{
		//构建Mesh
		//CBoxMesh* Box = CBoxMesh::CreatMesh();
		//CBoxMesh* Box = CBoxMesh::CreatMesh(2.f,1.f,1.5f);
		//CPlaneMesh* PlaneMesh = CPlaneMesh::CreateMesh(4.f, 3.f, 20, 20);

		//string MeshObjPath = "";
		//CCustomMesh* CustomMesh = CCustomMesh::CreateMesh(MeshObjPath);
		
		//CSphereMesh* SphereMesh = CSphereMesh::CreateMesh(2.f, 20, 20);
		//CCylinderMesh* CylinderMesh = CCylinderMesh::CreateMesh(1.f, 1.f, 5.f, 20, 20);
		CConeMesh* ConeMesh = CConeMesh::CreateMesh(1.f, 5.f, 20, 20);
		for (auto& Tmp : GObjects)
		{
			Tmp->BeginInit();
		}
	}

	ANALYSIS_HRESULT(GraphicsCommandList->Close());

	ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);
	WaitGPUCommandQueueComplete();
	return 0;
}

void CWindowsEngine::Tick(float DeltaTime) //每一帧都要刷新CommandList等
{
	for (auto& Tmp : GObjects)
	{
		if (Tmp->IsTick())
		{
			Tmp->Tick(DeltaTime);
		}
	}

	//重新录制相关的内容，为下一帧做准备
	ANALYSIS_HRESULT(CommandAllocator->Reset());

	//预渲染
	for (auto& Tmp : IRenderingInterface::RenderingInterface)
	{
		Tmp->PreDraw(DeltaTime);
	}


	//转换指向资源状态
	CD3DX12_RESOURCE_BARRIER ResourceBarrierPresent = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentSwapBuff(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	GraphicsCommandList->ResourceBarrier(1, &ResourceBarrierPresent);
	
	//每帧执行
	//绑定矩形框
	GraphicsCommandList->RSSetViewports(1, &ViewPortInfo);
	GraphicsCommandList->RSSetScissorRects(1, &ViewPortRect);

	//清除画布
	GraphicsCommandList->ClearRenderTargetView(GetCurrentSwapBufferView(), DirectX::Colors::Black, 0, nullptr);
	//清除深度模板缓冲区
	GraphicsCommandList->ClearDepthStencilView(GetCurrentDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, NULL);
	//输出的合并阶段
	D3D12_CPU_DESCRIPTOR_HANDLE SwapBufferView = GetCurrentSwapBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView = GetCurrentDepthStencilView();
	GraphicsCommandList->OMSetRenderTargets(1, &SwapBufferView, true, &DepthStencilView);
	
	//渲染其他内容

	for (auto& Tmp : IRenderingInterface :: RenderingInterface)
	{
		Tmp->Draw(DeltaTime);
		Tmp->PostDraw(DeltaTime);
	}


	CD3DX12_RESOURCE_BARRIER ResourceBarrierPresentRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentSwapBuff(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	GraphicsCommandList->ResourceBarrier(1, &ResourceBarrierPresentRenderTarget);
	//录入完成
	ANALYSIS_HRESULT(GraphicsCommandList->Close());
	//提交命令
	ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);
	//交换缓冲区
	ANALYSIS_HRESULT(SwapChain->Present(0, 0));
	CurrentSwapBuffIndex = !(bool)CurrentSwapBuffIndex;
	//CPU等GPU
	WaitGPUCommandQueueComplete();
}

int CWindowsEngine::PreExit()
{
	Engine_Log("Engine pre exit complete.");
	return 0;
}

int CWindowsEngine::Exit()
{
	Engine_Log("Engine exit complete.");
	return 0;
}

int CWindowsEngine::PostExit()
{
	FEngineRenderConfig::Destroy();
	Engine_Log("Engine post exit complete.");
	return 0;
}

ID3D12Resource* CWindowsEngine::GetCurrentSwapBuff() const
{
	return SwapChainBuffer[CurrentSwapBuffIndex].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE CWindowsEngine::GetCurrentSwapBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVHeap->GetCPUDescriptorHandleForHeapStart(), CurrentSwapBuffIndex, RTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE CWindowsEngine::GetCurrentDepthStencilView() const
{
	return DSVHeap->GetCPUDescriptorHandleForHeapStart();
}

UINT CWindowsEngine::GetDXGISampleCount() const
{
	return bMSAA4XEnabled ? 4 : 1;
}

UINT CWindowsEngine::GetDXGISampleQuality() const
{
	return bMSAA4XEnabled ? (M4XQualityLevels - 1) : 0;
}

void CWindowsEngine::WaitGPUCommandQueueComplete()
{
	CurrentFenceIndex++;
	//给GPU设置新的隔离点 等待GPU处理信号
	ANALYSIS_HRESULT(CommandQueue->Signal(Fence.Get(), CurrentFenceIndex));
	if (Fence->GetCompletedValue() < CurrentFenceIndex)
	{
		//创建或打开一个事件内核对象，并返回该对象的句柄
		HANDLE EventEX = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
		//GPU完成后通知Handle
		ANALYSIS_HRESULT(Fence->SetEventOnCompletion(CurrentFenceIndex, EventEX));
		//等待GPU，阻塞主线程
		WaitForSingleObject(EventEX, INFINITE);
		CloseHandle(EventEX);
	}
}

bool CWindowsEngine::InitWindows(FWinMainCommandParameters InParameters)
{
	//Registration window
	WNDCLASSEX WindowsClass;
	WindowsClass.cbSize = sizeof(WNDCLASSEX); //Space taken up
	WindowsClass.cbClsExtra = 0; //If need extra space
	WindowsClass.cbWndExtra = 0; //If need extra memory
	WindowsClass.hbrBackground = nullptr; //HDR backgroud erasing
	WindowsClass.hCursor = LoadCursor(NULL, IDC_ARROW); //Set mouse cursor
	WindowsClass.hIcon = nullptr; //An icon that displays the application on disk
	WindowsClass.hIconSm = NULL;//应用程序显示在左上角的图标
	WindowsClass.hInstance = InParameters.HInstance; // Window instance
	WindowsClass.lpszClassName = L"MyEngine"; //Window name
	WindowsClass.lpszMenuName = nullptr; //Menu name
	WindowsClass.style = CS_VREDRAW | CS_HREDRAW;  //Way of drawing window vertical and horizontal
	WindowsClass.lpfnWndProc = EngineWindowProc; //Processing message
	ATOM RegisterAtom = RegisterClassEx(&WindowsClass); //Sign the window
	if (!RegisterAtom)
	{
		Engine_Log_Error("Register windows class fail");
		MessageBox(NULL, L"Register windows class fail.", L"Error", MB_OK);
	}
	RECT Rect = { 0,0,FEngineRenderConfig::GetRenderConfig()->ScreenWidth,FEngineRenderConfig::GetRenderConfig() ->ScreenHeight}; //left top right bottom

	AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, NULL); //rect_Viewport   WS_ViewPort style   No menu

	int WindowWidth = Rect.right - Rect.left;
	int WindowHeight = Rect.bottom - Rect.top;

	MainWindowsHandle = CreateWindowEx(NULL,L"MyEngine", L"MyEngine", WS_OVERLAPPEDWINDOW, 100, 100, WindowWidth, WindowHeight, NULL, nullptr, InParameters.HInstance, NULL);
	//1.Windows extra style 2.Window name 3.Window title 4.Window style 5.Window coordinate 6.width 7.height 8.Deputy window handle 9.Menu handle 10.Window Instance 
	if (!MainWindowsHandle)
	{
		Engine_Log_Error("CreateWindow Failed..");
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}
	ShowWindow(MainWindowsHandle, SW_SHOW);
	UpdateWindow(MainWindowsHandle);

	Engine_Log("InitWindows complete");

	return true;
}

bool CWindowsEngine::InitDirect3D()
{
	//Debug
	ComPtr<ID3D12Debug>D3D12Debug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug))))
	{
		D3D12Debug->EnableDebugLayer();
	}
	//HRESULT
	//S_OK				0x00000000
	//E_UNEXPECTED		0x8000FFFF 意外的失败
	//E_NOTIMPL			0x80004001 未实现
	//E_OUTOFMEMORY		0x8007000E 未能分配所需要的内存
	//E_INVALIDARG		0x80070057 一个或多个参数无效
	//E_NOINTERFACE		0x80004002 不支持此接口
	//E_POINTER			0x80004003 无效指针 
	//E_HANDLE			0x80070006 无效句柄
	//E_ABORT			0x80004004 操作终止
	//E_FAIL			0x80004005 错误
	//E_ACCESSDENIED	0x80070005 一般的访问被拒绝错误

	ANALYSIS_HRESULT(CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory)));

	//D3D_FEATURE_LEVEL_11_0    支持D3D11.0    包含shader model 5
	HRESULT D3dDeviceResult = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3dDevice));
	if (FAILED(D3dDeviceResult))
	{
		//wrap
		ComPtr<IDXGIAdapter> WARPAdapter;
		ANALYSIS_HRESULT(DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&WARPAdapter)));
		//Software to  simulate the allocator
		ANALYSIS_HRESULT(D3D12CreateDevice(WARPAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3dDevice)));
	}
	/*
	Fence->SetEventOnCompletion
	执行命令
	提交呈现
	Queue->Signal
	wait
	*/

	ANALYSIS_HRESULT(D3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

	//Initialize command objects
	//NodeMask 指定命令队列应在哪个GPU节点上执行
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT; //里面储存内容直接被GPU执行
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	ANALYSIS_HRESULT(D3dDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));
	ID3D12CommandAllocator Allocator();
	ANALYSIS_HRESULT(D3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocator.GetAddressOf())));
	
	ANALYSIS_HRESULT(D3dDevice->CreateCommandList(0,//默认单个GPU
		D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, 
		CommandAllocator.Get(),//把CommandList关联到Allocator
		NULL, //管线状态 ID3D12PipelineState
		IID_PPV_ARGS(GraphicsCommandList.GetAddressOf())));

	ANALYSIS_HRESULT(GraphicsCommandList->Close());


	//////////////MSAA
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS QualityLevels;
	QualityLevels.Format = BackBufferFormat;
	QualityLevels.SampleCount = 4;
	QualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS::D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	QualityLevels.NumQualityLevels = 0;

	ANALYSIS_HRESULT(D3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &QualityLevels, sizeof(QualityLevels)));
	M4XQualityLevels = QualityLevels.NumQualityLevels;
	/////////////

	//交换链
	SwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferDesc.Width = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;
	SwapChainDesc.BufferDesc.Height = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = FEngineRenderConfig::GetRenderConfig()->RefreshRate;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferCount = FEngineRenderConfig::GetRenderConfig()->SwapChainCount;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //使用表面或资源作为渲染输出目标

	SwapChainDesc.OutputWindow = MainWindowsHandle; //指定Windows句柄
	SwapChainDesc.Windowed = true;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //随意切换窗口
	SwapChainDesc.BufferDesc.Format = BackBufferFormat;//纹理格式

	//多重采样
	SwapChainDesc.SampleDesc.Count = GetDXGISampleCount();
	SwapChainDesc.SampleDesc.Quality = GetDXGISampleQuality();
	ANALYSIS_HRESULT(DXGIFactory->CreateSwapChain(CommandQueue.Get(), &SwapChainDesc, SwapChain.GetAddressOf()));

	//资源描述符
	D3D12_DESCRIPTOR_HEAP_DESC RTVDescriptorHeapDesc;
	RTVDescriptorHeapDesc.NumDescriptors = 2;
	RTVDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTVDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	RTVDescriptorHeapDesc.NodeMask = 0;
	ANALYSIS_HRESULT(D3dDevice->CreateDescriptorHeap(&RTVDescriptorHeapDesc, IID_PPV_ARGS(RTVHeap.GetAddressOf())));
	
	D3D12_DESCRIPTOR_HEAP_DESC DSVDescriptorHeapDesc;
	DSVDescriptorHeapDesc.NumDescriptors = 1;
	DSVDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSVDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DSVDescriptorHeapDesc.NodeMask = 0;
	ANALYSIS_HRESULT(D3dDevice->CreateDescriptorHeap(&DSVDescriptorHeapDesc, IID_PPV_ARGS(DSVHeap.GetAddressOf())));
	
	return false;
}

void CWindowsEngine::PostInitDirect3D()
{
	WaitGPUCommandQueueComplete();
	ANALYSIS_HRESULT(GraphicsCommandList->Reset(CommandAllocator.Get(), NULL));
	for (int i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
	{
		SwapChainBuffer[i].Reset();
	}
	DepthStencilBuffer.Reset();

	SwapChain->ResizeBuffers(FEngineRenderConfig::GetRenderConfig()->SwapChainCount,
		FEngineRenderConfig::GetRenderConfig()->ScreenWidth,
		FEngineRenderConfig::GetRenderConfig()->ScreenHeight,
		BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	//拿到描述size 
	RTVDescriptorSize = D3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE HeapHandle(RTVHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < FEngineRenderConfig::GetRenderConfig()->SwapChainCount; i++)
	{
		SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer[i]));
		D3dDevice->CreateRenderTargetView(SwapChainBuffer[i].Get(), nullptr, HeapHandle);
		HeapHandle.Offset(1, RTVDescriptorSize);
	}

	D3D12_RESOURCE_DESC ResourceDesc;
	ResourceDesc.Width = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;
	ResourceDesc.Height = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;
	ResourceDesc.Alignment = 0;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.DepthOrArraySize = 1; //3D指定深度 2D或一下是数组的大小
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ResourceDesc.SampleDesc.Count = bMSAA4XEnabled ? 4 : 1;
	ResourceDesc.SampleDesc.Quality = bMSAA4XEnabled ? (M4XQualityLevels - 1) : 0;
	ResourceDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.DepthStencil.Depth = 1.f;
	ClearValue.DepthStencil.Stencil = 0;
	ClearValue.Format = DepthStencilFormat;

	CD3DX12_HEAP_PROPERTIES Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	D3dDevice->CreateCommittedResource(&Properties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COMMON,
		&ClearValue, IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf()));

	D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	DSVDesc.Format = DepthStencilFormat;
	DSVDesc.Texture2D.MipSlice = 0;
	DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
	D3dDevice->CreateDepthStencilView(DepthStencilBuffer.Get(), &DSVDesc, DSVHeap->GetCPUDescriptorHandleForHeapStart());

	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer.Get(), //资源同步：标明资源转换状态防止未读被写或未写被读
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	GraphicsCommandList->ResourceBarrier(1, &Barrier);

	GraphicsCommandList->Close();

	ID3D12CommandList* CommandList[] = { GraphicsCommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CommandList), CommandList);

	//覆盖原先画布的视口  描述视口尺寸
	ViewPortInfo.TopLeftX = 0;
	ViewPortInfo.TopLeftY = 0;
	ViewPortInfo.Width = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;
	ViewPortInfo.Height = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;
	ViewPortInfo.MinDepth = 0.f;
	ViewPortInfo.MaxDepth = 1.f;

	//视口矩形
	ViewPortRect.left = 0;
	ViewPortRect.top = 0;
	ViewPortRect.right = FEngineRenderConfig::GetRenderConfig()->ScreenWidth;
	ViewPortRect.bottom = FEngineRenderConfig::GetRenderConfig()->ScreenHeight;

	WaitGPUCommandQueueComplete();
}
  
#endif
