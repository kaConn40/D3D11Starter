#pragma once
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;
class Transform
{
	public:

		Transform();
		//getters
		XMFLOAT3 GetPosition();
		XMFLOAT3 GetPitchYawRoll(); 
		XMFLOAT3 GetScale();
		XMFLOAT4X4 GetWorldMatrix();
		XMFLOAT4X4 GetWorldInverseTransposeMatrix();

		//setters
		void SetPosition(float x, float y, float z);
		void SetPosition(DirectX::XMFLOAT3 pos);

		void SetRotation(float pitch, float yaw, float roll);
		void SetRotation(XMFLOAT3 rotation);

		void SetScale(float x, float y, float z);
		void SetScale(XMFLOAT3 scale);

		//transformers
		void MoveAbsolute(float x, float y, float z);
		void MoveAbsolute(XMFLOAT3 offset);
		void Rotate(float pitch, float yaw, float roll);
		void Rotate(XMFLOAT3 rotation);
		void Scale(float x, float y, float z);
		void Scale(XMFLOAT3 scale);

		void UpdateDirections();
		void UpdateMatrices();


	private:
		XMFLOAT3 position;
		XMFLOAT3 pitchYawRoll;
		XMFLOAT3 scale;

		bool isDirectionDirty;
		XMFLOAT3 up;
		XMFLOAT3 right;
		XMFLOAT3 foward;

		bool isMatrixDirty;
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 worldInverseMatrix;



		
	
};

