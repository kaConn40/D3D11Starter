#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
using namespace DirectX;

struct constBuffer
{
	XMFLOAT4 colorTint;
	XMFLOAT4X4 world;
	
};