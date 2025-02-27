TextureCube shadowSpecularTexture : register(t3);
SamplerComparisonState shadowSamplerHardware : register(s1);

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