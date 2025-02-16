//cbuffer RotationBuffer
//{
//    // row_major : HLSL�� ���켱, DirectX�� �� �켱�̱� ������ ��ġ�� �ϰų� row_major�� �ؾ� ����� ����� ���´�.
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