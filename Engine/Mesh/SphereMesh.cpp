#include "SphereMesh.h"
#include"Core/MeshType.h"

void CSphereMesh::Init()
{
	Super::Init();
}

void CSphereMesh::BuildMesh(const FMeshRenderingData* InRenderingData)
{
	Super::BuildMesh(InRenderingData);

}

void CSphereMesh::Draw(float DeltaTime)
{
	Super::Draw(DeltaTime);
}

CSphereMesh* CSphereMesh::CreateMesh(float InRadius, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
	FMeshRenderingData MeshData;

	float ThetaValue = XM_2PI / InHeightSubdivision;
	float BetaValue = XM_PI / InAxialSubdivision;

	MeshData.VertexData.push_back(FVertex(XMFLOAT3(0.f, InRadius, 0.f), XMFLOAT4(Colors::White)));

	for (size_t i = 1; i < InAxialSubdivision; ++i)
	{
		float Beta = i * BetaValue;

		for (size_t j = 0; j <= InHeightSubdivision; ++j)
		{
			float Theta = j * ThetaValue;
			MeshData.VertexData.push_back(FVertex(
				XMFLOAT3(
				InRadius * sinf(Beta) * cosf(Theta), //x
				InRadius * cosf(Beta),  //y
				InRadius * sinf(Beta) * sinf(Theta)),  //z
				XMFLOAT4(Colors::White)));

			int TopIndex = MeshData.VertexData.size() - 1;

			XMVECTOR Pos = XMLoadFloat3(&MeshData.VertexData[TopIndex].Position);         //Normal
			XMStoreFloat3(&MeshData.VertexData[TopIndex].Normal, XMVector3Normalize(Pos));

		}
	}

	MeshData.VertexData.push_back(FVertex(XMFLOAT3(0.f, -InRadius, 0.f), XMFLOAT4(Colors::White)));

	//绘制北极
	for (size_t Index = 0; Index < InAxialSubdivision; ++Index)
	{
		MeshData.IndexData.push_back(0);
		MeshData.IndexData.push_back(Index+1);
		MeshData.IndexData.push_back(Index);
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
	BaseIndex = SouthBaseIndex - InAxialSubdivision + 1;
	for (size_t Index = 0; Index < InAxialSubdivision; ++Index)
	{
		MeshData.IndexData.push_back(SouthBaseIndex);
		MeshData.IndexData.push_back(BaseIndex + Index);
		MeshData.IndexData.push_back(BaseIndex + Index + 1);
	}

	CSphereMesh* SphereMesh = new CSphereMesh;
	SphereMesh->BuildMesh(&MeshData);
	SphereMesh->Init();


	return NULL;
}
