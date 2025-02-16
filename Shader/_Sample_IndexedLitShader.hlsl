cbuffer LightInfoConstant
{
    float3 lightPosition;
    float3 ambient;         // �ֺ��� ����
    float3 diffuseColor;    // �л걤 ����
    float diffuseIntensity; // �л걤 ����
    float attConst;         // �� ���� ���
    float attLin;           // �� ���� ���� ����
    float attQuad;          // �� ��� ���� ����
};             

cbuffer ObjectColor
{
    float3 materialColors[6];   // ��ü�� ���� ����
    float padding;
    float specularIntensity;    // Specular ����
    float specularPower;        // Specular ����
};

float4 PS(float3 worldPosition : Position, float3 normal : Normal, uint triangleID : SV_PrimitiveID) : SV_Target
{
    // �������� ���� �Ÿ��� ������ ����
    const float3 vertexToLight = lightPosition - worldPosition;
    const float distance = length(vertexToLight);
    const float3 vertexToLightDir = vertexToLight / distance;
    
    // ���� ���� ������ ���ϰ� �л걤�� �Ÿ��� ���� ���� ������ ����
    const float attResult = 1.0f / (attConst + attLin * distance + attQuad * (distance * distance));
    const float3 diffuse = diffuseColor * diffuseIntensity * attResult * max(0.0f, dot(vertexToLightDir, normal));
    
    // ���� �ݻ�Ǵ� ������ ���� 
    const float3 reflectAngle = normal * dot(vertexToLight, normal);    // �븻 ���Ϳ� ������Ʈ���� ���� ������ ����
    const float3 reflectVector = reflectAngle * 2.0f - vertexToLight;   // ���� �ݻ�Ǵ� ������ ����
    
    // �ݻ籤 ��Ģ���� �ݻ籤�� ����
    const float3 specular = attResult * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-reflectVector), normalize(worldPosition))), specularPower);
    
    // �ֺ����� �л걤�� ��ģ �� 1.0�� �Ѿ ��� �ִ� 1.0���� ������
    return float4(saturate(diffuse + ambient + specular) * materialColors[(triangleID / 2) % 6],
    1.0f);
}