#include "ShaderInclude.hlsli"

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
SamplerState BasicSampler : register(s0); // "s" registers for sampler
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage


float4 main(VertexToPixel input) : SV_TARGET
{
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).xyz;
    surfaceColor *= float4(colorTint, 1);
   input.uv = input.uv * uvScale + uvOffset;
    input.normal = normalize(input.normal);
   
    float3 totalLight = ambient * surfaceColor.xyz;
    for (int i = 0; i < 5; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += Directional(light, input.normal, input.worldPos, cameraPosition, surfaceColor, roughness);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += Point(light, input.normal, input.worldPos, cameraPosition, surfaceColor, roughness);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += Spot(light, input.normal, input.worldPos, cameraPosition, surfaceColor, roughness);
                break;

        }
    }
    
    return float4(totalLight, 1);

}