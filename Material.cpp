#include "Material.h"


Material::Material(std::shared_ptr<SimplePixelShader> ps, std::shared_ptr<SimpleVertexShader> vs, DirectX::XMFLOAT4 colorTint, float roughness, DirectX::XMFLOAT2 uvOffset, DirectX::XMFLOAT2 uvScale) :
      ps(ps), vs(vs), colorTint(colorTint),uvOffset(uvOffset),uvScale(uvScale), roughness(roughness)
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

DirectX::XMFLOAT2 Material::GetUVOffset()
{
    return uvOffset;
}

DirectX::XMFLOAT2 Material::GetUVScale()
{
    return uvScale;
}

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Material::GetTextureSRVMap()
{
    return textureSRVs;
}

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> Material::GetSamplerMap()
{
    return samplers;
}

float Material::GetRoughness()
{
    return roughness;
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

void Material::AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
    textureSRVs.insert({ name,srv });
}

void Material::AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
    samplers.insert({ name,sampler });
}

void Material::SetUVOffset(DirectX::XMFLOAT2 offset)
{
    uvOffset = offset;
}

void Material::SetUVScale(DirectX::XMFLOAT2 scale)
{
    uvScale = scale;
}

void Material::SetRoughness(float roughness)
{
    //keep
    if (roughness > 1)
        roughness = 1;
    else if (roughness < 0)
        roughness = 0;


    this->roughness = roughness;
}

void Material::PrepareMaterials()
{
    ps->SetFloat2("uvOffset", uvOffset);
    ps->SetFloat2("uvScale", uvScale);
    ps->SetFloat("roughness", roughness);
    for (auto& t : textureSRVs)
    {
        ps->SetShaderResourceView(t.first.c_str(), t.second);
    }
    for (auto& s : samplers)
    {
        ps->SetSamplerState(s.first.c_str(), s.second);
    }
}
