#include "Transform.h"

using namespace DirectX;
Transform::Transform()
{
    position=XMFLOAT3(0.0f, 0.0f, 0.0f);
    pitchYawRoll = XMFLOAT3(0, 0, 0);
    scale = XMFLOAT3(1, 1, 1);

    right = XMFLOAT3(1, 0, 0);
    up = XMFLOAT3(0, 1, 0);
    foward = XMFLOAT3(0, 0, 1);

    isMatrixDirty = false;
    isDirectionDirty = false;

    XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&worldInverseMatrix, XMMatrixIdentity());
}


XMFLOAT3 Transform::GetPosition()
{
    return position;
}

XMFLOAT3 Transform::GetPitchYawRoll()
{
    return pitchYawRoll;
}

XMFLOAT3 Transform::GetScale()
{
    return scale;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
    UpdateDirections();
    return right;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
    UpdateDirections();
    return up;
}

DirectX::XMFLOAT3 Transform::GetFoward()
{
    UpdateDirections();
    return foward;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
    UpdateMatrices();
    return worldMatrix;
}

XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
    UpdateMatrices();
    return worldMatrix;
}

void Transform::SetPosition(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
    isMatrixDirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 pos)
{
    this->position = pos;
    isMatrixDirty = true;
   
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
    pitchYawRoll.x = pitch;
    pitchYawRoll.y = yaw;
    pitchYawRoll.z = roll;
    isMatrixDirty = true;
    isDirectionDirty = true;
    
}

void Transform::SetRotation(XMFLOAT3 rotation)
{
    SetRotation(rotation.x, rotation.y, rotation.z);
}

void Transform::SetScale(float x, float y, float z)
{
    scale.x = x;
    scale.y = y;
    scale.z = z;
    isMatrixDirty = true;
    

}

void Transform::SetScale(XMFLOAT3 scale)
{
    this->scale = scale;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
    position.x += x;
    position.y += y;
    position.z += z;
    isMatrixDirty = true;
}

void Transform::MoveAbsolute(XMFLOAT3 offset)
{
    MoveAbsolute(offset.x, offset.y, offset.z);
}

void Transform::MoveRelative(float x, float y, float z)
{
    XMVECTOR move=XMVectorSet(x, y, z, 0);
    XMVECTOR Qrot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
    XMVECTOR dir = XMVector3Rotate(move, Qrot);
    XMStoreFloat3(&position, XMLoadFloat3(&position) + dir);
    
}

void Transform::MoveRelative(DirectX::XMFLOAT3 offset)
{
    MoveRelative(offset.x, offset.y, offset.z);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
    pitchYawRoll.x += pitch;
    pitchYawRoll.y += yaw;
    pitchYawRoll.z += roll;
    isMatrixDirty = true;
    isDirectionDirty = true;
}

void Transform::Rotate(XMFLOAT3 rotation)
{
    pitchYawRoll.x += rotation.x;
    pitchYawRoll.y += rotation.y;
    pitchYawRoll.z += rotation.z;
    isMatrixDirty = true;
    isDirectionDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
    scale.x *= x;
    scale.y *= y;
    scale.z *= z;
    isMatrixDirty = true;
}

void Transform::Scale(XMFLOAT3 scaleBy)
{
    Scale(scaleBy.x, scaleBy.y, scaleBy.z);

}

void Transform::UpdateMatrices()
{
    //makes sure that it will only update when theres a change to the directions
    if (!isMatrixDirty)
        return;

    XMMATRIX tran = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
    XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
    XMMATRIX scal = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

    XMMATRIX mWrld = scal * rot * tran;

    XMStoreFloat4x4(&worldMatrix, mWrld);
    XMStoreFloat4x4(&worldInverseMatrix, XMMatrixInverse(0, XMMatrixTranspose(mWrld)));
    isMatrixDirty = false;
}

void Transform::UpdateDirections()
{
    //wont do calcs if theres no change 
    if (!isDirectionDirty)
        return;

   //get quad
    XMVECTOR qRot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
    //get new direction vectors
    //set them to the directions
    XMStoreFloat3(&up, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), qRot));
    XMStoreFloat3(&right, XMVector3Rotate(XMVectorSet(1, 0, 0, 0), qRot));
    XMStoreFloat3(&foward, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), qRot));
    //clear dirty
    isDirectionDirty = false;
    
}


