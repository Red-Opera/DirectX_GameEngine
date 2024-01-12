// WolrdViewProjection�� ��� ����
cbuffer ObjectWorldViewProjection
{
    float4 worldViewProjection;
};

void VertexShader(float3 position : POSITION, float4 color : COLOR, 
                  out float4 returnPosition : SV_Position, out float4 returnColor : COLOR)
{
    // ������ ���� ������ ���� ���� �������� �̵���
    returnPosition = mul(float4(position, 1.0f), worldViewProjection);
    
    // ���� ���� �׷��� ���� �ܰ�� ����
    returnColor = color;
}