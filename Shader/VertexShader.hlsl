// WolrdViewProjection�� ��� ����
// ��ü���� �����ؾ��ϱ� ������ ���� �󵵰� ���� ��� ����
cbuffer ObjectWorldViewProjection
{
    float4x4 worldViewProjection;
};

// ������ �����϶� �����ϱ� ������ ���� �󵵰� ������ ��� ����
cbuffer LightBuffer
{
    float3 lightDirection;
    float3 lightPosition;
    float3 lightColor;
};

// ���� �󵵰� ���� ���� ��� ����
cbuffer FogBuffer
{
    float4 fogColor;
    float fogStart;
    float fogEnd;
};

void VertexShader(float3 position : POSITION, float4 color : COLOR, 
                  out float4 returnPosition : SV_Position, out float4 returnColor : COLOR)
{
    // ������ ���� ������ ���� ���� �������� �̵���
    returnPosition = mul(float4(position, 1.0f), worldViewProjection);
    
    // ���� ���� �׷��� ���� �ܰ�� ����
    returnColor = color;
}

float4 PixelShder(float4 position : SV_Position, out float4 color : COLOR) : SV_Target
{
    // ������ ��ȯ�� ����� �Ѵ�.
    return color;
}