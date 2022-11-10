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
		//文件大小
		unsigned int FileSize = get_file_size_by_filename(InPath.c_str());
		//根据文件大小创建buffer
		char* Buff = new char[FileSize + 1];
		memset(Buff, 0, FileSize + 1);

		//提取Buffer
		get_file_buf(InPath.c_str(),Buff); //使用前初始化

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

		//读取一行数据
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
					//先添加一个
					MeshData.VertexData.push_back(FVertex(
						XMFLOAT3(), XMFLOAT4(Colors::White)));
					//得到添加后的位置
					int TopIndex = MeshData.VertexData.size() - 1;
					XMFLOAT3& Float3InPos = MeshData.VertexData[TopIndex].Position;

					//解析位置
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

					//输入一行数据
					LineStream >> SaveLineString;

					//找到索引位置
					int StringPosA = find_string(SaveLineString, "/", 0);

					memset(TmpBuff, 0, 256);
					char* VPosIndex = string_mid(SaveLineString, TmpBuff, 0, StringPosA);

					//放到索引容器
					MeshData.IndexData.push_back(atoi(VPosIndex));

					//纹理Index
					int StringPosB = find_string(SaveLineString, "/", StringPosA + 1);
					memset(TmpBuff, 0, 256);
					char* TexcoordIndex = string_mid(SaveLineString, TmpBuff, StringPosA + 1, StringPosB - (StringPosA + 1));

					//法线index
					memset(TmpBuff, 0, 256);
					char* NormalIndex = string_mid(SaveLineString, TmpBuff, StringPosB + 1, strlen(SaveLineString));
				}
			}
		}

	}
	return false;
}
