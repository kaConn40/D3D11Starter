#pragma once
#include <DirectXMath.h>
#include <vector>

class Transform
{
	public:

		Transform();
		//getters
		DirectX::XMFLOAT3 GetPosition();
		DirectX::XMFLOAT3 GetPitchYawRoll(); 
		DirectX::XMFLOAT3 GetScale();
		DirectX::XMFLOAT3 GetRight();
		DirectX::XMFLOAT3 GetUp();
		DirectX::XMFLOAT3 GetFoward();
		DirectX::XMFLOAT4X4 GetWorldMatrix();
		DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();

		//setters
		void SetPosition(float x, float y, float z);
		void SetPosition(DirectX::XMFLOAT3 pos);

		void SetRotation(float pitch, float yaw, float roll);
		void SetRotation(DirectX::XMFLOAT3 rotation);

		void SetScale(float x, float y, float z);
		void SetScale(DirectX::XMFLOAT3 scale);

		//transformers
		void MoveAbsolute(float x, float y, float z);
		void MoveAbsolute(DirectX::XMFLOAT3 offset);
		void MoveRelative(float x, float y, float z);
		void MoveRelative(DirectX::XMFLOAT3 offset);
		void Rotate(float pitch, float yaw, float roll);
		void Rotate(DirectX::XMFLOAT3 rotation);
		void Scale(float x, float y, float z);
		void Scale(DirectX::XMFLOAT3 scaleBy);

		void UpdateMatrices();
		void UpdateDirections();


	private:
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 pitchYawRoll;
		DirectX::XMFLOAT3 scale;

		DirectX::XMFLOAT3 up;
		DirectX::XMFLOAT3 right;
		DirectX::XMFLOAT3 foward;

		bool isMatrixDirty;
		bool isDirectionDirty;
		DirectX::XMFLOAT4X4 worldMatrix;
		DirectX::XMFLOAT4X4 worldInverseMatrix;



		
	
};

