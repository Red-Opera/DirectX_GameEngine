TextureCube shadowSpecularTexture : register(t3);
SamplerComparisonState shadowSamplerHardware : register(s1);

#define PCF_RANGE 3

static const float zf = 100.0f;
static const float zn = 0.5f;
static const float c1 = zf / (zf - zn);
static const float c0 = -zn * zf / (zf - zn);

float GetShadowDepth(const in float4 shadowPosition)
{
    // get magnitudes for each basis component
    const float3 m = abs(shadowPosition).xyz;
    
    // get the length in the dominant axis
    // (this correlates with shadow map face and derives comparison depth)
    const float major = max(m.x, max(m.y, m.z));
    
    // converting from distance in shadow light space to projected depth
    return (c1 * major + c0) / major;
}

float GetShadow(const in float4 shadowPosition)
{
    return shadowSpecularTexture.SampleCmpLevelZero(shadowSamplerHardware, shadowPosition.xyz, GetShadowDepth(shadowPosition));
}

//float GetShadow(const in float4 shadowPosition)
//{
//    float shadow = 0.0f;
//    
//    [unroll]
//    for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
//    {
//        [unroll]
//        for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
//        {
//            float3 offset = float3(x, y, 0.0f);
//            
//            shadow += shadowSpecularTexture.SampleCmpLevelZero(shadowSamplerHardware, shadowPosition.xyz + offset, GetShadowDepth(shadowPosition));
//        }
//    }
//        
//    return shadow / ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
//}