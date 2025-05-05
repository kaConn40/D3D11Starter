#pragma once
#include "Mesh.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <vector>
#include "GameEntity.h"
#include"Camera.h"
#include "Material.h"
#include "SimpleShader.h"
#include "Lights.h"
#include "Sky.h"
class Game
{
	
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();
	
private:
	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void CreateGeometry();
	void UpdateImGui(float deltaTime, float totalTime);
	void CreateShadowMap();
	void RenderShadowMap();
	void ResizePPRs();
	//some varaibles needed for ImGui
	DirectX::XMFLOAT4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
	std::unique_ptr<int>slider= std::make_unique<int>(50);
	std::unique_ptr<char[]>input= std::make_unique<char[]>(60);
	
	bool demoUI = false;
	DirectX::XMFLOAT4 colorTint = { 0.2f, 0.8f, 0.8f, 1.0f };
	std::vector < std::shared_ptr<Material>>matList;
	std::vector < std::shared_ptr<Mesh>>meshList;
	std::vector < std::shared_ptr<GameEntity>>entityList;
	std::vector <std::shared_ptr<Camera>>cams;
	std::shared_ptr<Camera>activeCam;
	DirectX::XMFLOAT3 ambientColor{ 0,0,0 };
	std::vector<Light>lights;
	std::shared_ptr<Sky> sky;

	//shadow
	std::shared_ptr<SimpleVertexShader> shadowVS;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	int shadowRes;
	float shadowProjSize;

	// Resources that are shared among all post processes
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	std::shared_ptr<SimpleVertexShader> ppVS;
	// Resources that are tied to a particular post process
	std::shared_ptr<SimplePixelShader> ppPS;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRTV; // For rendering
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ppSRV; // For samplin
	int blurRadius;
	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	


	
};

