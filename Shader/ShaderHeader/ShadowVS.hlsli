cbuffer ShadowConstantBuffer : register(b1)
{
    matrix shadowPosition;
};

float4 GetShadowHomoSpace(const in float3 position, uniform matrix modelTranform)
{
    const float4 shadowCamera = mul(float4(position, 1.0f), modelTranform);
    
    return mul(shadowCamera, shadowPosition);
}