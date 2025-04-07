#include "ShaderInclude.hlsli"
TextureCube CubeMap : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(Sky_VertexToPixel input) : SV_TARGET
{
    return CubeMap.Sample(BasicSampler, input.samplerDir);

}