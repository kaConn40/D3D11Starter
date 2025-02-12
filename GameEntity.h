#pragma once
#include "GameEntity.h"
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include "Mesh.h" 
#include"Transform.h"
#include "BufferStructs.h"
class GameEntity
{
	public:

		GameEntity(std::shared_ptr<Mesh> mesh);
		std::shared_ptr<Mesh> GetMesh();
		std::shared_ptr<Transform> GetTransform();
		
		void SetMesh(std::shared_ptr<Mesh>mesh);
		void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, constBuffer verShaderData);

	private:
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Transform> transform;
};

