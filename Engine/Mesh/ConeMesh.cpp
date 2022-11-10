#include "ConeMesh.h"
#include"Core/MeshType.h"

void CConeMesh::Init()
{
	Super::Init();
}

void CConeMesh::BuildMesh(const FMeshRenderingData* InRenderingData)
{
	Super::BuildMesh(InRenderingData);

}

void CConeMesh::Draw(float DeltaTime)
{
	Super::Draw(DeltaTime);
}

CConeMesh* CConeMesh::CreateMesh(float InRadius, float InHeight, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
	FMeshRenderingData MeshData;
	//半径间隔
	float RadiusInterval = -InRadius / (float)InHeightSubdivision;
	//高度间隔
	float HeightInterval = InHeight / (float)InHeightSubdivision;

	float BetaValue = XM_2PI / (float)InAxialSubdivision;


	//顶部中点
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(0.f, 0.5f * InHeight, 0.f), XMFLOAT4(Colors::White)));

	uint32_t Index = MeshData.VertexData.size();
	//float Y = -0.5f * InHeight;

	//底部圆
	for (uint32_t i = 0; i <= InAxialSubdivision; ++i)
	{
		float Y = 0.5f * InHeight - i * HeightInterval;
		float Radius = i * RadiusInterval;
		for (uint32_t j = 0; j <= InAxialSubdivision; ++j)
		{
			MeshData.VertexData.push_back(FVertex(
				XMFLOAT3(
					Radius * cosf(j * BetaValue),  //x
					Y, //y
					Radius * sinf(j * BetaValue)), //z
				XMFLOAT4(Colors::White)));
		}
	}

	//添加底部中点
	MeshData.VertexData.push_back(FVertex(XMFLOAT3(0.f, -0.5f * InHeight, 0.f), XMFLOAT4(Colors::White)));

	//绘制Index模型
	float CenterPoint = MeshData.VertexData.size() - 1;
	for (uint32_t i = 0; i < InAxialSubdivision; ++i)
	{
		MeshData.IndexData.push_back(0);
		MeshData.IndexData.push_back(Index + i + 1);
		MeshData.IndexData.push_back(Index + i);
	}

	float BaseIndex = 1;
	float VertexCircleNum = InAxialSubdivision + 1;
	for (uint32_t i = 0; i < InHeightSubdivision - 2; ++i)
	{
		for (uint32_t j = 0; j < InAxialSubdivision; ++j)
		{
			//绘制四边形
			//三角形1
			MeshData.IndexData.push_back(BaseIndex + i * VertexCircleNum + j);
			MeshData.IndexData.push_back(BaseIndex + i * VertexCircleNum + j + 1);
			MeshData.IndexData.push_back(BaseIndex + (i + 1) * VertexCircleNum + j);
			//三角形2
			MeshData.IndexData.push_back(BaseIndex + (i + 1) * VertexCircleNum + j);
			MeshData.IndexData.push_back(BaseIndex + i * VertexCircleNum + j + 1);
			MeshData.IndexData.push_back(BaseIndex + (i + 1) * VertexCircleNum + j + 1);
		}
	}

	//绘制南极
	uint32_t SouthBaseIndex = MeshData.VertexData.size() - 1;
	BaseIndex = SouthBaseIndex - VertexCircleNum;
	for (size_t Index = 0; Index < InAxialSubdivision; ++Index)
	{
		MeshData.IndexData.push_back(SouthBaseIndex);
		MeshData.IndexData.push_back(BaseIndex + Index);
		MeshData.IndexData.push_back(BaseIndex + Index + 1);
	}

	CConeMesh* ConeMesh = new CConeMesh;
	ConeMesh->BuildMesh(&MeshData);
	ConeMesh->Init();


	return ConeMesh;
}
