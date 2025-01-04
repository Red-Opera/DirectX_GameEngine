struct LightVector
{
    float3 vertexToLight;
    float3 vertexToLightDir;
    float distance;
};

LightVector GetLightVector(const in float3 lightPos, const in float3 position)
{
    LightVector lightVector;
    
    // �������� ���� �Ÿ��� ������ ����
    lightVector.vertexToLight = lightPos - position;
    lightVector.distance = length(lightVector.vertexToLight);
    lightVector.vertexToLightDir = lightVector.vertexToLight / lightVector.distance;
    
    return lightVector;
}