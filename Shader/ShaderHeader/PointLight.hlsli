cbuffer LightInfoConstant : register(b0)
{
    float3 lightViewPosition;
    float3 ambient;             // �ֺ��� ����
    float3 diffuseColor;        // �л걤 ����
    float diffuseIntensity;     // �л걤 ����
    float attConst;             // �� ���� ���
    float attLin;               // �� ���� ���� ����
    float attQuad;              // �� ��� ���� ����
};    