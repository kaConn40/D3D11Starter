#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Graphics.h"
#include "Vertex.h"
#include "fstream"
#include <stdexcept>
#include <memory> 
#include <DirectXMath.h>
#include <vector>
class Mesh
{
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
		const char* name;
		int indices;
		int vertices;

	public:
		//OOP
		Mesh(int vertNum,int indNum, Vertex* vertexList, unsigned int* indexList);
		Mesh(const char* name, const char* file);
		~Mesh();
		Mesh(const Mesh&) = delete; // Remove copy constructor
		Mesh& operator=(const Mesh&) = delete; // Remove copy-assignment operator


		//returns vertex buffer comptr
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();

		//returns indes buffer comptr
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

		//returns # of indices this mesh contains
		int GetIndexCount();

		//returns # of vertices this mesh contains
		int GetVertexCount();

		//sets buffers and draws using the correct number of indices
		void Draw();

		void CreateBuffers(Vertex* vertList,int vertNum,unsigned int* indList,int indNum);
		void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

};

