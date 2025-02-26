Texture2D shadowSpecularTexture : register(t3);
SamplerComparisonState shadowSamplerHardware : register(s1);

#define PCF_RANGE 1

float ShadowLoop(const in float3 shadowScreenPosition)
{
    float shadow = 0.0f;
    
    [unroll]
    for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
    {
        [unroll]
        for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
            shadow += shadowSpecularTexture.SampleCmpLevelZero(shadowSamplerHardware, shadowScreenPosition.xy, shadowScreenPosition.b, int2(x, y));
    }
        
    return shadow / ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
}

float GetShadow(const in float4 shadowHomoPosition)
{
    float shadow = 0.0f;
    const float3 shadowScreenPosition = shadowHomoPosition.xyz / shadowHomoPosition.w;
    
    if (shadowScreenPosition.z > 1.0f || shadowScreenPosition.z < 0.0f)
        shadow = 1.0f;
    
    else
        shadow = ShadowLoop(shadowScreenPosition);
    
    return shadow;
}