#pragma once
#include "Mesh.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <vector>

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
	void LoadShaders();
	void CreateGeometry();
	void UpdateImGui(float deltaTime, float totalTime);

	//some varaibles needed for ImGui
	//float* color;
	//int* slider;
	std::unique_ptr<float>color;
	std::unique_ptr<int>slider;
	std::unique_ptr<char[]>input;

	std::vector < std::shared_ptr<Mesh>>meshList;
	bool demoUI;
	//float* color;
	//int* slider;
	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	
};

