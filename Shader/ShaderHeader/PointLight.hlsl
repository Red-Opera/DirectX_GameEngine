cbuffer LightInfoConstant
{
    float3 lightViewPosition;
    float3 ambient;             // �ֺ��� ����
    float3 diffuseColor;        // �л걤 ����
    float diffuseIntensity;     // �л걤 ����
    float attConst;             // �� ���� ���
    float attLin;               // �� ���� ���� ����
    float attQuad;              // �� ��� ���� ����
};    