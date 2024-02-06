cbuffer WorldViewProjectionBuffer
{
    float4x4 worldViewProjection;
};

struct VertexInfoIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VertexInfoOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

VertexInfoOut VS(VertexInfoIn vertexInfo)
{
    VertexInfoOut vertexInfoOut;
    
    vertexInfoOut.pos = mul(float4(vertexInfo.pos, 1.0f), worldViewProjection);
    
    vertexInfoOut.color = vertexInfo.color;
    
    return vertexInfoOut;
}

float4 PS(VertexInfoOut vertexInfo) : SV_Target
{
    return vertexInfo.color;
}