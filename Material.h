#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>

#include "SimpleShader.h"
#include "Camera.h"
#include "Transform.h"
#include "Vertex.h"
#include <unordered_map> 
class Material
{
public:

	Material(std::shared_ptr<SimplePixelShader> ps, std::shared_ptr<SimpleVertexShader>vs, DirectX::XMFLOAT4 colorTint, float roughness, DirectX::XMFLOAT2 uvOffset = { 0,0 }, DirectX::XMFLOAT2 uvScale = { 1,1 });
	//getters
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	DirectX::XMFLOAT4 GetColorTint();
	DirectX::XMFLOAT2 GetUVOffset();
	DirectX::XMFLOAT2 GetUVScale();
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextureSRVMap();
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> GetSamplerMap();
	float GetRoughness();

	//setters
	void SetPixelShader(std::shared_ptr<SimplePixelShader> ps);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader>vs);
	void SetColorTint(DirectX::XMFLOAT4 colorTint);
	void AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>srv);
	void AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState>sampler);
	void SetUVOffset(DirectX::XMFLOAT2 offset);
	void SetUVScale(DirectX::XMFLOAT2 scale);
	void SetRoughness(float roughness);
	void PrepareMaterials();
private:
	std::shared_ptr<SimplePixelShader> ps;
	std::shared_ptr<SimpleVertexShader> vs;
	DirectX::XMFLOAT4 colorTint;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	DirectX::XMFLOAT2 uvOffset = { 0,0 };
	DirectX::XMFLOAT2 uvScale = { 1,1 };
	float roughness;

};

