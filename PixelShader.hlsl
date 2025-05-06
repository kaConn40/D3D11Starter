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
    bool useEmissive;
}
//textures and samplers
Texture2D Albedo : register(t0);
Texture2D NormalMap: register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalMap : register(t3);
Texture2D ShadowMap : register(t4);
Texture2D EmissiveMap : register(t5);

SamplerState BasicSampler : register(s0); 
SamplerComparisonState ShadowSampler : register(s1);



float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
  

    //uv
    input.uv = input.uv * uvScale + uvOffset;
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    input.normal = TransformNormal(input.normal, input.tangent, unpackedNormal);
    
    //alebedo
    float4 surfaceColor = pow(Albedo.Sample(BasicSampler, input.uv), 2.2);
    surfaceColor *= float4(colorTint, 1);
   
    //roughness
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    
    //metal
    float metal = MetalMap.Sample(BasicSampler, input.uv).r;
    
    float3 specularColor = lerp(F0_NON_METAL, surfaceColor.rgb, metal);
    
    float3 totalLight = ambient * surfaceColor.xyz;
    
        //shadows
////perspective divide
//    input.shadowMapPos /= input.shadowMapPos.w;
//// Convert the normalized device coordinates to UVs for sampling
//    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
//    shadowUV.y = 1.0f - shadowUV.y; // Flip the Y
//// Grab the distances we need: light-to-pixel and closest-surface
//    float distToLight = input.shadowMapPos.z;
// // Get a ratio of comparison results using SampleCmpLevelZero()
    //float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, distToLight).r;
    for (int i = 0; i < 5; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += Directional(light, input.normal, input.worldPos, cameraPosition, surfaceColor.xyz, roughness, metal);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += Point(light, input.normal, input.worldPos, cameraPosition, surfaceColor.xyz, roughness,metal);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += Spot(light, input.normal, input.worldPos, cameraPosition, surfaceColor.xyz, roughness,metal);
                break;

        }
    }
    if (useEmissive)
    {
        float3 emissiveColor = EmissiveMap.Sample(BasicSampler, input.uv).rgb;
        totalLight += pow(emissiveColor.rgb, 2.2);

    }
    else
    {
        totalLight += float3(0, 0, 0);

    }
    float3 final = pow(totalLight, 1 / 2.2f);
    return float4(final,1);

}