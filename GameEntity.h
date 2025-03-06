#pragma once
#include "GameEntity.h"
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include "Mesh.h" 
#include"Transform.h"
#include "BufferStructs.h"
#include "Camera.h"
#include "Material.h"

class GameEntity
{
	public:

		GameEntity(std::shared_ptr<Mesh> mesh,std::shared_ptr<Material> mat);
		std::shared_ptr<Mesh> GetMesh();
		std::shared_ptr<Transform> GetTransform();
		std::shared_ptr<Material> GetMaterial();
		
		void SetMesh(std::shared_ptr<Mesh>mesh);
		void Draw(std::shared_ptr<Camera> camera);
		void SetMaterial(std::shared_ptr<Material> mat);

	private:
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Transform> transform;
		std::shared_ptr<Material> mat;
};

