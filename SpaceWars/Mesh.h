#include "DXCore.h"
#include "Vertex.h"
#include <fstream>
#include <vector>

#pragma once
class Mesh
{
private:
	// Buffers to hold actual geometry data
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

	void InitBuffers(UINT* indices, Vertex* verticies, int indexCount, int vertexCount, ID3D11Device* device);
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();

	//Mesh Managment
	static void loadMeshes(ID3D11Device* device);
	static Mesh* getByIndex(int index);
	static void release();
};

