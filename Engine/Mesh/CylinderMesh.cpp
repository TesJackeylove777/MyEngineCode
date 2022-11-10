#include "CylinderMesh.h"
#include"Core/MeshType.h"

void CCylinderMesh::Init()
{
	Super::Init();
}

void CCylinderMesh::BuildMesh(const FMeshRenderingData* InRenderingData)
{
	Super::BuildMesh(InRenderingData);

}

void CCylinderMesh::Draw(float DeltaTime)
{
	Super::Draw(DeltaTime);
}

CCylinderMesh* CCylinderMesh::CreateMesh(float InTopRadius, float InBottomRadius, float InHeight, uint32_t InAxialSubdivision, uint32_t InHeightSubdivision)
{
	FMeshRenderingData MeshData;
	//�뾶���
	float RadiusInterval = (InTopRadius - InBottomRadius) / InHeightSubdivision;
	//�߶ȼ��
	float HeightInterval = InHeight / InHeightSubdivision;

	float BetaValue = XM_2PI / (float)InAxialSubdivision;

	for (uint32_t i = 0; i < InHeightSubdivision; ++i)
	{
		float Y = 0.5f * InHeight - HeightInterval * i;
		float Radius = InTopRadius + i * RadiusInterval;

		for (uint32_t j = 0; j <= InAxialSubdivision; ++j)
		{
			//��������ת��Ϊ�ѿ�������ϵ
			MeshData.VertexData.push_back(FVertex(
				XMFLOAT3(
					Radius * cosf(j * BetaValue),  //x
					Y, //y
					Radius * sinf(j * BetaValue)), //z
				XMFLOAT4(Colors::White)));
		}
	}


	float VertexCircleNum = InAxialSubdivision + 1;
	for (uint32_t i = 0; i < InHeightSubdivision; ++i)
	{
		for (uint32_t j = 0; j < InAxialSubdivision; ++j)
		{
			//�����ı���
			//����Զ�������
			//������1
			//MeshData.IndexData.push_back(i * VertexCircleNum + j);
			//MeshData.IndexData.push_back((i + 1) * VertexCircleNum + j);
			//MeshData.IndexData.push_back((i + 1) * VertexCircleNum + j + 1);
			//������2
			//MeshData.IndexData.push_back(i * VertexCircleNum + j);
			//MeshData.IndexData.push_back((i + 1) * VertexCircleNum + j + 1);
			//MeshData.IndexData.push_back(i * VertexCircleNum + j + 1);

			//���߿��������
			//������1
			MeshData.IndexData.push_back((i + 1) * VertexCircleNum + j + 1);
			MeshData.IndexData.push_back((i + 1) * VertexCircleNum + j);
			MeshData.IndexData.push_back(i * VertexCircleNum + j);

			//������2
			MeshData.IndexData.push_back(i * VertexCircleNum + j + 1);
			MeshData.IndexData.push_back((i + 1) * VertexCircleNum + j + 1);
			MeshData.IndexData.push_back(i * VertexCircleNum + j);
		}
	}

	//��������
	if(1)
	{
		uint32_t Index = MeshData.VertexData.size();
		float Y = 0.5f * InHeight;
		for (uint32_t i = 0; i <= InAxialSubdivision; ++i)
		{
			MeshData.VertexData.push_back(FVertex(
				XMFLOAT3(
					InTopRadius * cosf(i * BetaValue),  //x
					Y, //y
					InTopRadius * sinf(i * BetaValue)), //z
				XMFLOAT4(Colors::White)));
		}
		//�����е�
		MeshData.VertexData.push_back(FVertex(XMFLOAT3(0.f,Y,0.f),XMFLOAT4(Colors::White)));

		//����Indexģ��
		float CenterPoint = MeshData.VertexData.size() - 1;
		for (uint32_t i = 0; i < InAxialSubdivision; ++i)
		{
			MeshData.IndexData.push_back(CenterPoint);
			MeshData.IndexData.push_back(Index + i + 1);
			MeshData.IndexData.push_back(Index + i);
		}
	}

	//�����ײ�
	if(1)
	{
		uint32_t Index = MeshData.VertexData.size();
		float Y = -0.5f * InHeight;
		for (uint32_t i = 0; i <= InAxialSubdivision; ++i)
		{
			MeshData.VertexData.push_back(FVertex(
				XMFLOAT3(
					InBottomRadius * cosf(i * BetaValue),  //x
					Y, //y
					InBottomRadius * sinf(i * BetaValue)), //z
				XMFLOAT4(Colors::White)));
		}
		//�����е�
		MeshData.VertexData.push_back(FVertex(XMFLOAT3(0.f, Y, 0.f), XMFLOAT4(Colors::White)));

		//����Indexģ��
		float CenterPoint = MeshData.VertexData.size() - 1;
		for (uint32_t i = 0; i < InAxialSubdivision; ++i)
		{
			MeshData.IndexData.push_back(CenterPoint);
			MeshData.IndexData.push_back(Index + i);
			MeshData.IndexData.push_back(Index + i + 1);
		}
	}
	CCylinderMesh* CylinderMesh = new CCylinderMesh;
	CylinderMesh->BuildMesh(&MeshData);
	CylinderMesh->Init();


	return NULL;
}