#include "GameEntity.h"
using namespace std;
using namespace DirectX;

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh)
{
    this->mesh = mesh;
    transform = make_shared<Transform>();
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
    return mesh;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
    return transform;
}

void GameEntity::SetMesh(std::shared_ptr<Mesh> mesh)
{
    this->mesh = mesh;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer,constBuffer verShaderData,std::shared_ptr<Camera> camera)
{
    verShaderData.world = transform->GetWorldMatrix();
    verShaderData.projection = camera->GetProjection();
    verShaderData.view = camera->GetView();

    D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
    Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

    memcpy(mappedBuffer.pData, &verShaderData, sizeof(verShaderData));
    Graphics::Context->Unmap(constantBuffer.Get(), 0);
    mesh->Draw();
}
