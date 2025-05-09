
cbuffer ExternalData : register(b0)
{
    float pixelWidth;
    float pixelHeight;
    int blurRadius;
}

// Defines the input to this pixel shader
struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// Textures and such
Texture2D PixelColors : register(t0);
SamplerState BasicSampler : register(s0);

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
    float3 color = 0;
    int totalSamples = 0;
	
    for (int x = -blurRadius; x <= blurRadius; x++)
    {
        for (int y = -blurRadius; y <= blurRadius; y++)
        {
            float2 uv = input.uv;
            uv.x += x * pixelWidth;
            uv.y += y * pixelHeight;
			
            color += PixelColors.Sample(BasicSampler, uv).rgb;
            totalSamples++;
        }
    }
	
    return float4(color / totalSamples, 1);
}