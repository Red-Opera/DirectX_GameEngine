cbuffer ShadowConstantBuffer : register(b1)
{
    matrix shadowViewProjection;
};

float4 GetShadowHomoSpace(const in float3 position, uniform matrix modelTranform)
{
    const float4 shadowCamera = mul(float4(position, 1.0f), modelTranform);
    const float4 shadowHomo = mul(shadowCamera, shadowViewProjection);
    
    return shadowHomo * float4(0.5f, -0.5f, 1.0f, 1.0f) + float4(0.5f, 0.5f, 0.0f, 0.0f) * shadowHomo.w;
}