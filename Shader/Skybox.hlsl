cbuffer TransformConstantBuffer : register(b0)
{
    matrix viewProjection;
};

struct VSOut
{
    float3 worldPosition : Position;
    float4 position : SV_Position;
};

TextureCube skyboxTexture : register(t0);
SamplerState samplerState : register(s0);

VSOut VS(float3 position : Position)
{
    VSOut output;
    
    output.worldPosition = position;
    output.position = mul(float4(position, 0.0f), viewProjection);
    output.position.z = output.position.w;
    
    return output;
}

float4 PS(float3 worldPosition : Position) : SV_TARGET
{
    return skyboxTexture.Sample(samplerState, worldPosition);
}