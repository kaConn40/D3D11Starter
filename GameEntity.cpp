#include "GameEntity.h"
using namespace std;
using namespace DirectX;

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat)
{
    this->mesh = mesh;
    this->mat = mat;
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

std::shared_ptr<Material> GameEntity::GetMaterial()
{
    return mat;
}

void GameEntity::SetMesh(std::shared_ptr<Mesh> mesh)
{
    this->mesh = mesh;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> mat)
{
    this->mat = mat;
}
void GameEntity::Draw(std::shared_ptr<Camera> camera)
{
    std::shared_ptr<SimpleVertexShader> vs = mat->GetVertexShader();
    std::shared_ptr<SimplePixelShader> ps = mat->GetPixelShader();
    vs->SetShader();
    ps->SetShader();
    XMFLOAT4 color = mat->GetColorTint();

    vs->SetMatrix4x4("world", transform->GetWorldMatrix());
    vs->SetMatrix4x4("view", camera->GetView());
    vs->SetMatrix4x4("projection", camera->GetProjection());
    vs->CopyAllBufferData();

    ps->SetFloat3("colorTint", &color.x);
    ps->CopyAllBufferData();
    
    mesh->Draw();
}


