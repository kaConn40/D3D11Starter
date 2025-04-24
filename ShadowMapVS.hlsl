#include "ShaderInclude.hlsli"

cbuffer externalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
}

float4 main( VertexShaderInput input ) : SV_POSITION
{
    matrix mvp = mul(projection, mul(view, world));
    return mul(mvp, float4(input.localPosition, 1.0f));

}