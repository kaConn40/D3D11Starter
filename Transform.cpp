#include "Transform.h"

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
    pitchYawRoll = rotation;
    isMatrixDirty = true;
    isDirectionDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
    scale.x = x;
    scale.y = y;
    scale.z = z;
    isMatrixDirty = true;
    isDirectionDirty = true;

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
    position.x += offset.x;
    position.y += offset.y;
    position.z += offset.z;
    isMatrixDirty = true;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{

}

void Transform::Rotate(XMFLOAT3 rotation)
{
}

void Transform::Scale(float x, float y, float z)
{
}

void Transform::Scale(XMFLOAT3 scale)
{
}

void Transform::UpdateDirections()
{
}

void Transform::UpdateMatrices()
{
}
