cbuffer ConstantBuffer
{
    matrix transform;
};

struct VertexOut
{
    float4 color : Color;
    float4 pos : SV_Position;
};

VertexOut VS(float3 position : Position, float4 color : Color)
{
    VertexOut vertexOut;
    
    vertexOut.pos = mul(float4(position, 1.0f), transform);
    vertexOut.color = color;
    
    return vertexOut;
}

float4 PS(float4 color : Color) : SV_Target
{
    return color;
}