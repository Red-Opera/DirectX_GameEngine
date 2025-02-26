#include "ShaderHeader/Transform.hlsli"

cbuffer Color : register(b1)
{
    float3 materialColor;
};

float4 VS(float3 position : Position) : SV_Position
{
    return mul(float4(position, 1.0f), worldViewProjection);
}

float4 PS() : SV_Target
{
    return float4(materialColor, 1.0f);
}