#include "Mesh.h"



Mesh::Mesh(int vertNum, int indNum, Vertex* vertexList, unsigned int* indexList)
{
	this->indices = indNum;
	this->vertices = vertNum;

	// Create a VERTEX BUFFER
	// - This holds the vertex data of triangles for a single object
	// - This buffer is created on the GPU, which is where the data needs to
	//    be if we want the GPU to act on it (as in: draw it to the screen)
	{
		D3D11_BUFFER_DESC vtb = {};
		vtb.Usage = D3D11_USAGE_IMMUTABLE;	// Will NEVER change
		vtb.ByteWidth = sizeof(Vertex) * vertices;       //* number of vertices in the buffer
		vtb.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Tells Direct3D this is a vertex buffer
		vtb.CPUAccessFlags = 0;	// Note: We cannot access the data from C++ (this is good)
		vtb.MiscFlags = 0;
		vtb.StructureByteStride = 0;
		// Create the proper struct to hold the initial vertex data
		// - This is how we initially fill the buffer with data
		// - Essentially, we're specifying a pointer to the data to copy
		D3D11_SUBRESOURCE_DATA initialVertexData = {};
		initialVertexData.pSysMem = vertexList; // pSysMem = Pointer to System Memory

		// Actually create the buffer on the GPU with the initial data
		// - Once we do this, we'll NEVER CHANGE DATA IN THE BUFFER AGAIN
		Graphics::Device->CreateBuffer(&vtb, &initialVertexData, this->vertexBuffer.GetAddressOf());
	}

	// Create an INDEX BUFFER
	// - This holds indices to elements in the vertex buffer
	// - This is most useful when vertices are shared among neighboring triangles
	// - This buffer is created on the GPU, which is where the data needs to
	//    be if we want the GPU to act on it (as in: draw it to the screen)
	{
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;	// Will NEVER change
		ibd.ByteWidth = sizeof(unsigned int) * indices;	// * number of indices in the buffer
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;	// Tells Direct3D this is an index buffer
		ibd.CPUAccessFlags = 0;	// Note: We cannot access the data from C++ (this is good)
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		// Specify the initial data for this buffer, similar to above
		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = indexList; // pSysMem = Pointer to System Memory

		// Actually create the buffer with the initial data
		// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
		Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	}
}

Mesh::~Mesh()
{
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
    return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
    return indexBuffer;
}

int Mesh::GetIndexCount()
{
    return indices;
}

int Mesh::GetVertexCount()
{
    return vertices;
}

void Mesh::Draw()
{
	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Draw this mesh
	Graphics::Context->DrawIndexed(this->indices, 0, 0);
}
