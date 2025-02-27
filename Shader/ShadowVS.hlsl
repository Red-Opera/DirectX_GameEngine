#include "ShaderHeader/Transform.hlsli"

float4 VS(float3 position : Position) : SV_Position
{
    return mul(float4(position, 1.0f), worldViewProjection);
}