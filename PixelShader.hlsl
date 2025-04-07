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

Texture2D SurfaceTexture : register(t0); // "t" registers for textures
Texture2D NormalMap: register(t1);

SamplerState BasicSampler : register(s0); // "s" registers for sampler
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage


float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
  
    input.uv = input.uv * uvScale + uvOffset;
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    input.normal = TransformNormal(input.normal, input.tangent, unpackedNormal);
    
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv);
    surfaceColor *= float4(colorTint, 1);
   
    float3 totalLight = ambient * surfaceColor.xyz;
    for (int i = 0; i < 5; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += Directional(light, input.normal, input.worldPos, cameraPosition, surfaceColor.xyz, roughness);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += Point(light, input.normal, input.worldPos, cameraPosition, surfaceColor.xyz, roughness);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += Spot(light, input.normal, input.worldPos, cameraPosition, surfaceColor.xyz, roughness);
                break;

        }
    }
    
    return float4(totalLight, 1);

}