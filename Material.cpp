#include "Material.h"

Material::Material(std::shared_ptr<SimplePixelShader> ps, std::shared_ptr<SimpleVertexShader> vs, DirectX::XMFLOAT4 colorTint) :
    ps(ps), vs(vs), colorTint(colorTint)
{
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
    return ps;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
    return vs;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
    return colorTint;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> ps)
{
    this->ps = ps;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vs)
{
    this->vs = vs;
}

void Material::SetColorTint(DirectX::XMFLOAT4 colorTint)
{
    this->colorTint = colorTint;
}
