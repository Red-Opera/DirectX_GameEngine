// �ش� ��ü�� Transfrom�� �����ϱ����� cbuffer
cbuffer TransformBuffer
{
    matrix transform;
};

struct Vertex
{
    float2 textCoord : TexCoord;        // �ؽ�ó ��ǥ ��ġ
    float4 position : SV_Position;
};

// Texture2D  texgture : register(t0)���� ������ �� ����
Texture2D inputTexture;          // �ؽ�ó�� ���� �ؽ�ó
SamplerState samplerState;  // Sampler State���� ������ ��

Vertex VS(float3 position : Position, float2 textCoord : TexCoord)
{
    Vertex vertex;
    vertex.position = mul(float4(position, 1.0f), transform);
    vertex.textCoord = textCoord;                               // ������ �ؽ�ó ��ǥ�� �Է���
    
    return vertex;
}

float4 PS(float2 textCoord : TexCoord) : SV_Target
{
    // �ش� �ؽ�ó���� Sampler State�� ������ ����� ��ȯ��
    return inputTexture.Sample(samplerState, textCoord);
}