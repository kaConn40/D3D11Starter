#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>

#include "SimpleShader.h"
#include "Camera.h"
#include "Transform.h"
#include "Vertex.h"

class Material
{
public:

	Material(std::shared_ptr<SimplePixelShader> ps, std::shared_ptr<SimpleVertexShader>vs, DirectX::XMFLOAT4 colorTint);
	//getters
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	DirectX::XMFLOAT4 GetColorTint();

	//setters
	void SetPixelShader(std::shared_ptr<SimplePixelShader> ps);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader>vs);
	void SetColorTint(DirectX::XMFLOAT4 colorTint);

private:
	std::shared_ptr<SimplePixelShader> ps;
	std::shared_ptr<SimpleVertexShader> vs;
	DirectX::XMFLOAT4 colorTint;

};

