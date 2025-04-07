#ifndef _GGP_LIGHTING_
#define _GPP_LIGHTING_

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2 

#define MAX_SPECULAR_EXPONENT 256.0f

struct Light
{
    int Type; //what kind of light (0,1,2)
    float3 Direction; //directional and spot need direction

    float Range; //point and spot have max range 
    float3 Position; //point and spot has pos in space

    float Intensity; //all need intensity
    float3 Color; //all need color

    float SpotInnerAngle; //inner cone angle(rads)
    float SpotOuterAngle; //outer cone angle(rads)
    float2 Padding; //padding to hit 16 bit boundry

};
float Diffuse(float3 normal, float3 direction)
{
    return saturate(dot(normal, direction));

}
float Specular(float3 normal, float3 lightDirection, float3 camDirection, float roughness)
{
    float3 refl = reflect(-lightDirection, normal);
    float specEx = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    if (specEx < 0.05)
        return 0;
    return pow(max(dot(refl, camDirection), 0.0f), specEx);
}
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - dist * dist / light.Range * light.Range);
    return att * att;

}

float3 Directional(Light light, float3 normal, float3 worldPos, float3 camPos, float3 surfaceCol, float roughness)
{
    //get directions
    float3 lightDirection = normalize(-light.Direction);
    float3 camDirection = normalize(camPos - worldPos);
    //calc lights
    float diffuse = Diffuse(normal, lightDirection);
    float spec = Specular(normal, lightDirection, camDirection, roughness);
    spec *= any(diffuse);
    return (diffuse * surfaceCol + spec) * light.Intensity * light.Color;

}
float3 Point(Light light, float3 normal, float3 worldPos, float3 camPos, float3 surfaceCol, float roughness)
{
    //get directions
    float3 lightDirection = normalize(light.Position - worldPos);
    float3 camDirection = normalize(camPos - worldPos);
    
    //calc lights
    float att = Attenuate(light, worldPos);
    float diff = Diffuse(normal, lightDirection);
    float spec = Specular(normal, lightDirection, camDirection, roughness);
    spec *= any(diff);
    return (diff * surfaceCol + spec) * att * light.Intensity * light.Color;
    

}
float3 Spot(Light light, float3 normal, float3 worldPos, float3 camPos, float3 surfaceCol, float roughness)
{
    float3 toPixel = normalize(light.Position - worldPos);
    float pixelAgnle = saturate(dot(-toPixel, light.Direction));
    
    float cosOut = cos(light.SpotOuterAngle);
    float cosIn = cos(light.SpotInnerAngle);
    float falloff = cosOut - cosIn;
    
    float spotTerm = saturate((cosOut - pixelAgnle) / falloff);
    return Point(light, normal, worldPos, camPos, surfaceCol, roughness) * spotTerm;

}
float3 TransformNormal(float3 normal, float3 tangent, float3 unpackedNormal)
{
    float3 N = normalize(normal);
    float3 T = normalize(tangent - N * dot(tangent, N));
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    return normalize(mul(unpackedNormal, TBN));

}
#endif