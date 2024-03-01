cbuffer ObjectTransform
{
    matrix transform;
    matrix worldViewProjection;
};

cbuffer Color
{
    float4 color;
};

float4 VS(float3 position : Position) : SV_Position
{
    return mul(float4(position, 1.0f), worldViewProjection);
}

float4 PS() : SV_Target
{
    return color;
}