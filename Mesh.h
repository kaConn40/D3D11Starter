#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Graphics.h"
#include "Vertex.h"

#include <memory> 
class Mesh
{
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

		int indices;
		int vertices;

	public:
		//OOP
		Mesh(int vertNum,int indNum, Vertex* vertexList, unsigned int* indexList);
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

};

