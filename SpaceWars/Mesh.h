#include "DXCore.h"
#include "Vertex.h"
#include <fstream>
#include <vector>

using namespace DirectX;

#pragma once
class Mesh
{
private:
	// Buffers to hold actual geometry data
	std::vector<XMFLOAT3> positions;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int indexCount;

	//Mesh Management
	//TODO: convert this to map based mesh management
	static const int MESH_COUNT = 6;
	static Mesh** meshes;

public:

	Mesh(UINT indices[], Vertex vertices[], int indexCount, int vertexCount, ID3D11Device* device);
	Mesh(char* modelName, ID3D11Device* device);
	~Mesh();

	void initBuffers(UINT* indices, Vertex* verticies, int indexCount, int vertexCount, ID3D11Device* device);
	ID3D11Buffer* getVertexBuffer();
	ID3D11Buffer* getIndexBuffer();
	std::vector<XMFLOAT3> getVertices();
	int getIndexCount();

	//Mesh Managment
	static void loadMeshes(ID3D11Device* device);
	static Mesh* getByIndex(int index);
	static void release();
};

