#pragma once
#include <DirectXMath.h>
#include <vector>
#include <Windows.h>
#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


using namespace DirectX;


class Geometrie
{
private:
	struct HeightMap
	{
		XMFLOAT3 pos;
		XMFLOAT3 n;
		XMFLOAT3 map;
	};

	int terrainHeight, terrainWidth;
	float heightScale;
	char* terrainFilename,* colorMapFilename;
	HeightMap* heightMap;

public:

	struct Vertex 
	{ 
		Vertex() 
		{

		}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
		: Position(p), Normal(n), TangentU(t), TexC(uv)
		{

		}
		Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v) 
		: Position(px, py, pz), Normal(nx, ny, nz), TangentU(tx, ty, tz), TexC(u, v) 
		{

		}
		XMFLOAT3 Position; 
		XMFLOAT3 Normal; 
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC; 
		XMFLOAT3 Color;
	};
	struct MeshData 
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
		std::vector<std::wstring> Textures;

		
	};
	struct Vertici
	{
		XMFLOAT3 posizione;
		XMFLOAT2 texcoord;
		XMFLOAT3 normal;
		XMFLOAT3 color;
	};
	struct VerticiColorati
	{
		XMFLOAT3 posizione;
		XMFLOAT4 colore;
	};
	struct uiVertici
	{
		XMFLOAT2 posizione;
		XMFLOAT2 texcoord;
	};
	std::vector<MeshData> meshes;

	Geometrie();
	~Geometrie();

	void CreateTriangleGrid(float width, float depth, UINT m, UINT n, MeshData& meshData);
	bool CreateHeightMapGrid(char*,MeshData&);

	// assimp loader 
	void processMesh(aiMesh* mesh, const aiScene* scene);
	void processoRicorsivo(aiNode* node, const aiScene* scene);
	bool CreaMesh(const char*,bool);

	// height maps

	bool LoadSetupFile(char*);
	bool LoadBitmapHeightMap();
	void ShutdownHeightMap();
	void SetTerrainCoordinates();
	bool BuildTerrainModel(MeshData&);
	bool CalculateNormals();

	// color maps
	bool LoadColorMap();
};

