#include "ShaderInclude.hlsli"
#include "LightsInclude.hlsli"
cbuffer ExternalData : register(b0)
{
    float3 colorTint;
    float roughness;
    float2 uvOffset;
    float2 uvScale;
    float3 cameraPosition;
    float3 ambient;
    Light lights[5];
}
Texture2D NormalMap : register(t0);
TextureCube CubeMap : register(t1);

SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input):SV_TARGET
{
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
  
    input.uv = input.uv * uvScale + uvOffset;
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    input.normal = TransformNormal(input.normal, input.tangent, unpackedNormal);
   
    float3 viewVector = normalize(cameraPosition - input.worldPos);
    float3 reflVector = reflect(-viewVector, input.normal);
    float3 reflectionColor = CubeMap.Sample(BasicSampler, reflVector).rgb;
    return float4(reflectionColor, 1);
}