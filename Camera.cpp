#include "Camera.h"
#include "Input.h"
using namespace std;
using namespace DirectX;
using namespace Input;
Camera::Camera(DirectX::XMFLOAT3 pos, float fov, float aspectRatio, float nearClip, float farClip, float moveSpeed, float mouseLookSpeed, bool isActive)  :
	fov(fov),aspectRatio(aspectRatio),nearClip(nearClip),farClip(farClip),moveSpeed(moveSpeed),mouseLookSpeed(mouseLookSpeed),isActive(isActive)
{
	transform = make_shared<Transform>();
	transform->SetPosition(pos);
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}
XMFLOAT4X4 Camera::GetView()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjection()
{
	return projMatrix;
}

shared_ptr<Transform> Camera::GetTransform()
{
	return transform;
}

float Camera::GetFOV()
{
	return fov;
}
void Camera::SetFOV(float fov)
{
	this->fov = fov;
}
void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	this->aspectRatio = aspectRatio;

	XMMATRIX pM=XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
	XMStoreFloat4x4(&projMatrix, pM);
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 foward = transform->GetFoward();
	XMFLOAT3 pos = transform->GetPosition();
	

	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&pos), XMLoadFloat3(&foward), XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&viewMatrix, view);

}

void Camera::Update(float deltaTime)
{
	//makes sure that only the active cam can move

	if (!isActive)
		return;

	float speed = deltaTime * moveSpeed;
	//movement
	if (KeyDown('W')){transform->MoveRelative(0, 0, speed);}
	if (KeyDown('S')){transform->MoveRelative(0, 0, -speed);}
	if (KeyDown('A')){transform->MoveRelative(-speed, 0, 0);}
	if (KeyDown('D')){transform->MoveRelative(speed, 0, 0);}
	if (KeyDown('X')) { transform->MoveAbsolute(0, -speed, 0); }
	if (KeyDown(' ')) { transform->MoveAbsolute(0, speed, 0); }


	//mouse input
	if (MouseLeftDown())
	{
		float cursorX = GetMouseXDelta()*mouseLookSpeed;
		float cursorY = GetMouseYDelta()*mouseLookSpeed;
		transform->Rotate(cursorY, cursorX, 0);


		XMFLOAT3 rot = transform->GetPitchYawRoll();
		if (rot.x > XM_PIDIV2) rot.x = XM_PIDIV2; 
		if (rot.x < -XM_PIDIV2) rot.x = -XM_PIDIV2; 
		transform->SetRotation(rot);


	}
	UpdateViewMatrix();
}




