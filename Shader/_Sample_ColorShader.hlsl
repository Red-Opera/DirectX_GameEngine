//cbuffer RotationBuffer
//{
//    // row_major : HLSL은 열우선, DirectX는 행 우선이기 때문에 전치를 하거나 row_major를 해야 제대로 결과가 나온다.
//    //row_major matrix rotation;
//    
//    matrix rotation;
//};
cbuffer ConstantBuffer
{
    matrix transform;
}

cbuffer FaceColorBuffer
{
    float4 faceColor[8];
};

float4 VS(float3 position : Position) : SV_Position
{
    return mul(float4(position, 1.0f), transform);
}

float4 PS(uint triangleId : SV_PrimitiveID) : SV_Target
{
    return faceColor[(triangleId / 2) % 8];
}