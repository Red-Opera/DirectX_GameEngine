TextureCube shadowSpecularTexture : register(t3);
SamplerComparisonState shadowSamplerHardware : register(s1);

#define PCF_RANGE 3              // PCF 필터링 범위 (픽셀 단위)

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

// PCSS 그림자 함수로 구현
float GetShadow(const in float4 shadowPosition)
{
    // 블로커 탐색: 작은 영역에서 평균 블로커 깊이 계산
    float avgBlockerDepth = 0.0f;
    int blockerCount = 0;
    const int blockerKernel = 1; // 탐색 영역 크기

    [unroll]
    for (int x = -blockerKernel; x <= blockerKernel; x++)
    {
        [unroll]
        for (int y = -blockerKernel; y <= blockerKernel; y++)
        {
            if (x < 0 || y < 0)
                continue;
            
            float3 offset = float3(x, y, 0.0f) * 0.002f; // 오프셋 스케일 조정
            float currentDepth = GetShadowDepth(shadowPosition);
            float sample = shadowSpecularTexture.SampleCmpLevelZero(shadowSamplerHardware, shadowPosition.xyz + offset, currentDepth);

            if (sample < currentDepth)
            {
                avgBlockerDepth += sample;
                blockerCount++;
            }
        }
    }

    float receiverDepth = GetShadowDepth(shadowPosition);

    // 블로커가 없을 때, avgBlockerDepth를 receiverDepth로 설정하여 경계값 계산 오류를 방지
    if (blockerCount > 0)
        avgBlockerDepth /= blockerCount;
    
    else
        avgBlockerDepth = receiverDepth;
    
    float penumbraRatio = saturate((receiverDepth - avgBlockerDepth) / avgBlockerDepth);
    float filterRadius = penumbraRatio * PCF_RANGE;
    int intFilterRadius = max(1, int(filterRadius));

    // PCSS 필터링: 필터링 영역 내에서 샘플링
    float shadow = 0.0f;
    int sampleCount = 0;

    [unroll]
    for (int i = -intFilterRadius; i <= intFilterRadius; i++)
    {
        [unroll]
        for (int j = -intFilterRadius; j <= intFilterRadius; j++)
        {
            if (i >= 0 && j >= 0)
            {
                float3 offset = float3(i, j, 0.0f) * 0.03f;
                shadow += shadowSpecularTexture.SampleCmpLevelZero(shadowSamplerHardware, shadowPosition.xyz + offset, receiverDepth);
                sampleCount++;
            }
        }
    }
    
    return shadow / sampleCount;
}
