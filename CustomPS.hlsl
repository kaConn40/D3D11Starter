cbuffer ExternalData : register(b0)
{
    float3 colorTint;
    float time;
}

struct VertexToPixel
{
	
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};


//make sin wave
float4 main(VertexToPixel input) : SV_TARGET
{
    float total = 0;
    float height = (input.uv.y * -2 + 1) * 10.0f;
    float freq = 2 * 3.14 * 2.5f;
    for (int i = 0; i < 4;i++)
    {
        if (i > 0)
        {
           height = (input.uv.y * -2 + 1) * 10.0f * (i * 3);
        }
           
        float t = sin(input.uv.x * freq + time * 2 + sin(time) * 2.75f + (i * 3));
        float dist = 1 - saturate(pow(abs(height - t), 1.0f));
        total += dist;
    }
        
    
    return float4(colorTint * total, 1);
    

}