#include "PlaneMesh.h"
#include"Core/MeshType.h"

void CPlaneMesh::Init()
{
	Super::Init();
}

void CPlaneMesh::BuildMesh(const FMeshRenderingData* InRenderingData)
{
	Super::BuildMesh(InRenderingData);

}

void CPlaneMesh::Draw(float DeltaTime)
{
	Super::Draw(DeltaTime);
}

CPlaneMesh* CPlaneMesh::CreateMesh(float InHeight, float InWidth, uint32_t InHeightSubdivide, uint32_t InWidthSubdivide)
{
	FMeshRenderingData MeshData;

	auto SubdivideValue = [&](float InValue, uint32_t InSubdivideValue)->float
	{
		if (InSubdivideValue <= 1)
		{
			return InValue;
		}
		return InHeight / ((float)InHeightSubdivide - 1);
	};

	float CHeight = 0.5f * InHeight;
	float CWidth = 0.5f * InWidth;

	float HeightSubdivideValue = SubdivideValue(InHeight, InHeightSubdivide);
	float WidthSubdivideValue = SubdivideValue(InWidth, InWidthSubdivide);

	for (uint32_t i = 0; i < InHeightSubdivide; ++i)
	{
		float Z = CHeight - i * HeightSubdivideValue;
		for (uint32_t j = 0; j < InWidthSubdivide; ++j)
		{
			float X = CWidth - j * WidthSubdivideValue;
			MeshData.VertexData.push_back(FVertex(
				XMFLOAT3(
					X,
					0.f,
					Z),
				XMFLOAT4(Colors::White)));
		}
	}

	//����index
	for (uint32_t i = 0; i < InHeightSubdivide - 1; ++i)
	{
		for (uint32_t j = 0; j < InWidthSubdivide - 1; ++j)
		{
			//�����ı���
			//������1
			MeshData.IndexData.push_back((i + 1) * InWidthSubdivide + j);
			MeshData.IndexData.push_back(i * InWidthSubdivide + j + 1);
			MeshData.IndexData.push_back(i * InWidthSubdivide + j);

			//������2
			MeshData.IndexData.push_back((i + 1) * InWidthSubdivide + j + 1);
			MeshData.IndexData.push_back(i * InWidthSubdivide + j + 1);
			MeshData.IndexData.push_back((i + 1) * InWidthSubdivide + j);
		}
	}

	CPlaneMesh* PlaneMesh = new CPlaneMesh;
	PlaneMesh->BuildMesh(&MeshData);
	PlaneMesh->Init();


	return NULL;
}
