cbuffer ShadowControl : register(b2)
{
	int pcfLevel;
	float shadowBias;
	bool isUseHardwarePCF;
};

Texture2D shadowSpecularTexture : register(t3);
SamplerComparisonState shadowSamplerHardware : register(s1);
SamplerState shadowSamplerSoftware : register(s2);

float ShadowLoop(const in float3 shadowScreenPosition, uniform int pcfRange)
{
	float shadow = 0.0f;
    
    [unroll]
	for (int x = -pcfRange; x <= pcfRange; x++)
	{
        [unroll]
		for (int y = -pcfRange; y <= pcfRange; y++)
		{
			if (isUseHardwarePCF)
				shadow += shadowSpecularTexture.SampleCmpLevelZero(shadowSamplerHardware, shadowScreenPosition.xy, shadowScreenPosition.b - shadowBias, int2(x, y));
			else
				shadow += shadowSpecularTexture.Sample(shadowSamplerSoftware, shadowScreenPosition.xy + int2(x, y)).r >= shadowScreenPosition.b - shadowBias ? 1.0f : 0.0f;
		}
	}
        
	return shadow / ((pcfRange * 2 + 1) * (pcfRange * 2 + 1));
}

float GetShadow(const in float4 shadowHomoPosition)
{
	float shadow = 0.0f;
	const float3 shadowScreenPosition = shadowHomoPosition.xyz / shadowHomoPosition.w;
    
	if (shadowScreenPosition.z > 1.0f || shadowScreenPosition.z < 0.0f)
		shadow = 1.0f;
	
	else
	{
        [unroll]
		for (int level = 0; level <= 4; level++)
		{
			if (level == pcfLevel)
				shadow = ShadowLoop(shadowScreenPosition, level);
		}
	}
    
	return shadow;
}