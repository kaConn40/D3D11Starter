#pragma once
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2 
#include <DirectXMath.h>
struct Light
{
	int Type; //what kind of light (0,1,2)

	DirectX::XMFLOAT3 Direction; //directional and spot need direction

	float Range; //point and spot have max range 

	DirectX::XMFLOAT3 Position; //point and spot has pos in space

	float Intensity; //all need intensity

	DirectX::XMFLOAT3 Color; //all need color

	float SpotInnerAngle; //inner cone angle(rads)

	float SpotOuterAngle; //outer cone angle(rads)

	DirectX::XMFLOAT2 Padding; //padding to hit 16 bit boundry

};

