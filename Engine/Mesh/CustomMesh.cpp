#include "CustomMesh.h"
#include"Core/MeshType.h"

void CCustomMesh::Init()
{
	Super::Init();
}

void CCustomMesh::BuildMesh(const FMeshRenderingData* InRenderingData)
{
	Super::BuildMesh(InRenderingData);

}

void CCustomMesh::Draw(float DeltaTime)
{
	Super::Draw(DeltaTime);
}

CCustomMesh* CCustomMesh::CreateMesh(string& InPath)
{
	FMeshRenderingData MeshData;

	{
		//�ļ���С
		unsigned int FileSize = get_file_size_by_filename(InPath.c_str());
		//�����ļ���С����buffer
		char* Buff = new char[FileSize + 1];
		memset(Buff, 0, FileSize + 1);

		//��ȡBuffer
		get_file_buf(InPath.c_str(),Buff); //ʹ��ǰ��ʼ��

		if (!LoadObjFormBuff(Buff, FileSize, MeshData))
		{

		}
		delete Buff;
	}

	CCustomMesh* CustomMesh = new CCustomMesh;
	CustomMesh->BuildMesh(&MeshData);
	CustomMesh->Init();


	return CustomMesh;
}

bool CCustomMesh::LoadObjFormBuff(char* InBuff, uint32_t InBuffSize, FMeshRenderingData& MeshData)
{
	stringstream BuffStream(InBuff);
	char TmpLine[256] = { 0 };
	string MidTmpTag;

	for (; !BuffStream.eof();)
	{
		memset(TmpLine, 0, 256);

		//��ȡһ������
		BuffStream.getline(TmpLine, 256);
		if (strlen(TmpLine) > 0)
		{
			if (TmpLine[0] == 'v')
			{
				stringstream LineStream(TmpLine);
				LineStream >> MidTmpTag;
				if (TmpLine[1] == 'n')
				{

				}
				else if (TmpLine[1] == 't')
				{

				}
				else
				{
					//�����һ��
					MeshData.VertexData.push_back(FVertex(
						XMFLOAT3(), XMFLOAT4(Colors::White)));
					//�õ���Ӻ��λ��
					int TopIndex = MeshData.VertexData.size() - 1;
					XMFLOAT3& Float3InPos = MeshData.VertexData[TopIndex].Position;

					//����λ��
					LineStream >> Float3InPos.x;
					LineStream >> Float3InPos.y;
					LineStream >> Float3InPos.z;
				}
			}
			else if (TmpLine[0] == 'f')
			{
				stringstream LineStream(TmpLine);
				LineStream >> MidTmpTag;

				char SaveLineString[256] = { 0 };
				char TmpBuff[256] = { 0 };
				for (size_t i = 0; i < 3; i++)
				{
					memset(SaveLineString, 0, 256);

					//����һ������
					LineStream >> SaveLineString;

					//�ҵ�����λ��
					int StringPosA = find_string(SaveLineString, "/", 0);

					memset(TmpBuff, 0, 256);
					char* VPosIndex = string_mid(SaveLineString, TmpBuff, 0, StringPosA);

					//�ŵ���������
					MeshData.IndexData.push_back(atoi(VPosIndex));

					//����Index
					int StringPosB = find_string(SaveLineString, "/", StringPosA + 1);
					memset(TmpBuff, 0, 256);
					char* TexcoordIndex = string_mid(SaveLineString, TmpBuff, StringPosA + 1, StringPosB - (StringPosA + 1));

					//����index
					memset(TmpBuff, 0, 256);
					char* NormalIndex = string_mid(SaveLineString, TmpBuff, StringPosB + 1, strlen(SaveLineString));
				}
			}
		}

	}
	return false;
}
