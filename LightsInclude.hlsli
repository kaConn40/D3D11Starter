#ifndef _GGP_LIGHTING_
#define _GPP_LIGHTING_

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2 

#define MAX_SPECULAR_EXPONENT 256.0f

// A constant Fresnel value for non-metals (glass and plastic have values of about 0.04)
static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal

// Handy to have this as a constant
static const float PI = 3.14159265359f;
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

// Calculates diffuse amount based on energy conservation
//
// diffuse - Diffuse amount
// F - Fresnel result from microfacet BRDF
// metalness - surface metalness amount
float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - dist * dist / light.Range * light.Range);
    return att * att;

}
 // Normal Distribution Function: GGX (Trowbridge-Reitz)
    //
    // a - Roughness
    // h - Half vector: (V + L)/2
    // n - Normal
    //
    // D(h, n, a) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float D_GGX(float3 n, float3 h, float roughness)
{
    // Pre-calculations
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!
    
    // ((n dot h)^2 * (a^2 - 1) + 1)
    // Can go to zero if roughness is 0 and NdotH is 1; MIN_ROUGHNESS helps here
    float denomToSquare = NdotH2 * (a2 - 1) + 1;
    
    // Final value
    return a2 / (PI * denomToSquare * denomToSquare);
}
    // Fresnel term - Schlick approx.
    //
    // v - View vector
    // h - Half vector
    // f0 - Value when l = n
    //
    // F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 F_Schlick(float3 v, float3 h, float3 f0)
{
    float VdotH = saturate(dot(v, h));
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}
    // Geometric Shadowing - Schlick-GGX
    // - k is remapped to a / 2, roughness remapped to (r+1)/2 before squaring!
    //
    // n - Normal
    // v - View vector
    // G_Schlick(n,v,a) = (n dot v) / ((n dot v) * (1 - k) * k)
float G_SchlickGGX(float3 n, float3 v, float roughness)
{
// End result of remapping:
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));
// Final value
// Note: Numerator should be NdotV (or NdotL depending on parameters).
// However, these are also in the BRDF's denominator, so they'll cancel!
// We're leaving them out here AND in the BRDF function as the
// dot products can get VERY small and cause rounding errors.
    return 1 / (NdotV * (1 - k) + k);
}

    // Cook-Torrance Microfacet BRDF (Specular)
    //
    // f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
    // - parts of the denominator are canceled out by numerator (see below)
    //
    // D() - Normal Distribution Function - Trowbridge-Reitz (GGX)
    // F() - Fresnel - Schlick approx
    // G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0, float3 F_out)
{
    // Other vectors
    float3 h = normalize(v + l); // That�s an L, not a 1! Careful copy/pasting from a PDF!
    // Run numerator functions
    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    float G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);
    // Pass F out of the function for diffuse balance
    F_out = F;
    // Final specular formula
    float3 specularResult = (D * F * G) / 4;
    
    // One last non-obvious requirement: According to the rendering equation,
    // specular must have the same NdotL applied as diffuse! We'll apply
    // that here so that minimal changes are required elsewhere.
    return specularResult * max(dot(n, l), 0);
}

float3 Directional(Light light, float3 normal, float3 worldPos, float3 camPos, float3 surfaceCol, float roughness,float metal)
{
    //get directions
    float3 lightDirection = normalize(-light.Direction);
    float3 camDirection = normalize(camPos - worldPos);
    float3 F = 0;
    //calc lights
    float diffuse = Diffuse(normal, lightDirection);
    float3 spec = MicrofacetBRDF(normal, lightDirection, camDirection, roughness, surfaceCol, F);
    
    //get energy consrvation
    float3 balDiff = DiffuseEnergyConserve(diffuse, F, metal);
    return (balDiff * surfaceCol + spec) * light.Intensity * light.Color;

}
float3 Point(Light light, float3 normal, float3 worldPos, float3 camPos, float3 surfaceCol, float roughness,float metal)
{
    //get directions
    float3 lightDirection = normalize(light.Position - worldPos);
    float3 camDirection = normalize(camPos - worldPos);
    float3 F = 0;
    //calc lights
    float att = Attenuate(light, worldPos);
    float diff = Diffuse(normal, lightDirection);
    float3 spec = MicrofacetBRDF(normal, lightDirection, camDirection, roughness, surfaceCol, F);
    //get energy consrvation
    float3 balDiff = DiffuseEnergyConserve(diff, F, metal);
    return (balDiff * surfaceCol + spec) * att * light.Intensity * light.Color;
    

}
float3 Spot(Light light, float3 normal, float3 worldPos, float3 camPos, float3 surfaceCol, float roughness,float metal)
{
    float3 toPixel = normalize(light.Position - worldPos);
    float pixelAgnle = saturate(dot(-toPixel, light.Direction));
    
    float cosOut = cos(light.SpotOuterAngle);
    float cosIn = cos(light.SpotInnerAngle);
    float falloff = cosOut - cosIn;
    
    float spotTerm = saturate((cosOut - pixelAgnle) / falloff);
    return Point(light, normal, worldPos, camPos, surfaceCol, roughness,metal) * spotTerm;

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