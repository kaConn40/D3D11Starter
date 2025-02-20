#pragma once
#include "DirectXMath.h"
#include "Transform.h"
#include <memory>
class Camera
{
	public: 
		Camera(DirectX::XMFLOAT3 pos, float fov,float aspectRatio, float nearClip, float farClip, float moveSpeed, float mouseLookSpeed, bool isActive); 
		~Camera();
		bool isActive;

		DirectX::XMFLOAT4X4 GetView();
		DirectX::XMFLOAT4X4 GetProjection();
		std::shared_ptr<Transform>GetTransform();
		float GetFOV();
		void SetFOV(float fov);

		void UpdateProjectionMatrix(float aspectRatio);
		void UpdateViewMatrix();
		void Update(float deltaTime);

		
	private:
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projMatrix;
		std::shared_ptr<Transform> transform;


		float fov;
		float nearClip;
		float farClip;
		float moveSpeed;
		float mouseLookSpeed;
		float aspectRatio;
		
	

};



