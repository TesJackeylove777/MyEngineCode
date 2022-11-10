#include"Rendering.h"
#include"../../Debug/EngineDebug.h"
#include"../../Platform/Windows/WindowsEngine.h"
vector<IRenderingInterface*> IRenderingInterface:: RenderingInterface;

IRenderingInterface::IRenderingInterface()
{
	RenderingInterface.push_back(this);
}

IRenderingInterface::~IRenderingInterface()
{
	for (vector<IRenderingInterface*>::const_iterator Iter = RenderingInterface.begin(); Iter != RenderingInterface.end(); ++Iter)
	{
		if (*Iter == this)
		{
			RenderingInterface.erase(Iter);
			break;
		}
	}
}

void IRenderingInterface::Init()
{
}

void IRenderingInterface::PreDraw(float DeltaTime)
{
	//重置CommandList
	ANALYSIS_HRESULT(GetGraphicsCommandList()->Reset(GetCommandAllocator().Get(), NULL));
}

void IRenderingInterface::Draw(float DeltaTime)
{
}

void IRenderingInterface::PostDraw(float DeltaTime)
{
}

ComPtr<ID3D12Resource> IRenderingInterface::ConstructDefaultBuffer(ComPtr<ID3D12Resource>& OutTmpBufffer, const void* InData, UINT64 InDataSize)
{
	ComPtr<ID3D12Resource> Buffer;
	CD3DX12_RESOURCE_DESC BufferResourceDESC = CD3DX12_RESOURCE_DESC::Buffer(InDataSize);
	CD3DX12_HEAP_PROPERTIES BufferProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ANALYSIS_HRESULT(GetD3dDevice()->CreateCommittedResource(&BufferProperties, D3D12_HEAP_FLAG_NONE,
		&BufferResourceDESC, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(Buffer.GetAddressOf())));

	CD3DX12_HEAP_PROPERTIES UpdateBufferProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	ANALYSIS_HRESULT(GetD3dDevice()->CreateCommittedResource(&UpdateBufferProperties, D3D12_HEAP_FLAG_NONE,
		&BufferResourceDESC, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(OutTmpBufffer.GetAddressOf())));

	D3D12_SUBRESOURCE_DATA SubResourceData = {};
	SubResourceData.pData = InData;
	SubResourceData.RowPitch = InDataSize;
	SubResourceData.SlicePitch = SubResourceData.RowPitch;

	//把资源标记为复制目标
	CD3DX12_RESOURCE_BARRIER CopyDestBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Buffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

	GetGraphicsCommandList()->ResourceBarrier(1, &CopyDestBarrier);

	UpdateSubresources<1>(GetGraphicsCommandList().Get(),
		Buffer.Get(), OutTmpBufffer.Get(), 0, 0, 1, &SubResourceData);

	CD3DX12_RESOURCE_BARRIER ReadDestBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Buffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

	return Buffer;
}

ComPtr<ID3D12Device> IRenderingInterface::GetD3dDevice()
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		return InEngine->D3dDevice;
	}
	return NULL;
}

ComPtr<ID3D12GraphicsCommandList> IRenderingInterface::GetGraphicsCommandList()
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		return InEngine->GraphicsCommandList;
	}
	return NULL;
}

ComPtr<ID3D12CommandAllocator> IRenderingInterface::GetCommandAllocator()
{
	if (CWindowsEngine* InEngine = GetEngine())
	{
		return InEngine->CommandAllocator;
	}
	return NULL;
}

#if defined(_WIN32)
CWindowsEngine* IRenderingInterface::GetEngine()
{
	return dynamic_cast<CWindowsEngine*>(Engine);
}
#else
FEngine *IRenderingInterface::GetEngine()
{
	return Engine;
}
#endif




FRenderingResourcesUpdate::FRenderingResourcesUpdate()
{

}

FRenderingResourcesUpdate::~FRenderingResourcesUpdate()
{
	if (UploadBuffer != nullptr)
	{
		UploadBuffer->Unmap(0, NULL);
		UploadBuffer = nullptr;
	}
}

void FRenderingResourcesUpdate::Init(ID3D12Device* InDevice, UINT InElementSize, UINT InElementCount)
{
	assert(InDevice);
	ElementSize = InElementSize;
	CD3DX12_HEAP_PROPERTIES HeapPropertie = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(InElementSize * InElementCount);
	InDevice->CreateCommittedResource(&HeapPropertie, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&UploadBuffer));
	ANALYSIS_HRESULT(UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&Data)));
}

void FRenderingResourcesUpdate::Update(int Index, const void* InData)
{
	memcpy(&Data[Index * ElementSize], InData, ElementSize);
}

UINT FRenderingResourcesUpdate::GetConstantBufferByteSize(UINT InTypeSize)
{
	return (InTypeSize + 255)& ~255;
}

UINT FRenderingResourcesUpdate::GetConstantBufferByteSize()
{
	return GetConstantBufferByteSize(ElementSize);
}
