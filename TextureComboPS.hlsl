cbuffer ExternalData : register(b0)
{
    float3 colorTint;
    float2 uvOffset;
    float2 uvScale;
}

Texture2D SurfaceTexture : register(t0); // "t" registers for textures
Texture2D SpectralTexture : register(t1);

SamplerState BasicSampler : register(s0); // "s" registers for sampler
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};


float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    input.uv = input.uv * uvScale + uvOffset;
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv);
    float4 specColor = SpectralTexture.Sample(BasicSampler, input.uv).r;
    surfaceColor *= float4(colorTint, 1);
    surfaceColor *= specColor;
    return surfaceColor;
}