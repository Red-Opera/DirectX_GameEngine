#include "ShaderHeader/Transform.hlsl"

cbuffer Offset
{
    float offset;
};

float4 VS(float3 position : Position, float3 normal : Normal) : SV_Position
{
    return mul(float4(position + normal * offset, 1.0f), worldViewProjection);
}