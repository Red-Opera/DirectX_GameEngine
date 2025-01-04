// �ش� �ȼ��� �ش��ϴ� Local Normal ���� View Noraml ������ �����Ű�� �Լ�
float3 GetViewNormal(const in float3 tangent, const in float3 biTangent, const in float3 normal,
                     const in float2 textureCoord, uniform Texture2D normalMap, uniform SamplerState state)
{
    // Tangent, biTangent, Normal ���� �ϳ��� ��ķ� ǥ��
    // TangentX,    TangentY,   TangetZ
    // biTangentX,  biTangentY, biTangentZ
    // NormalX,     NormalY,    NormalZ
    const float3x3 tangetToView = float3x3(tangent, biTangent, normal);
        
    // ������ [0, 1] -> [-1, 1]
    const float3 normalSample = normalMap.Sample(state, textureCoord).xyz;
    const float3 tangentNormal = normalSample * 2.0f - 1.0f;
        
    // ���� ����(Tangent Space)���� �� ����(View Speace)�� viewNormal�� ������
    return normalize(mul(tangentNormal, tangetToView));
}

// �Ÿ��� ���� ���� �� ���⸦ ���ϴ� �Լ�
float GetAttenuate(uniform float attConst, uniform float attLin, uniform float attQuad, const in float distance)
{
    return 1.0f / (attConst + attLin * distance + attQuad * (distance * distance));
}

// �Ÿ��� ���� �л걤 ���⸦ ���ϴ� �Լ�
float3 GetDiffuse(uniform float3 diffuseColor, uniform float diffuseIntensity, const in float attResult, const in float3 vertexToLightDir, const in float3 viewNormal)
{
    return diffuseColor * diffuseIntensity * attResult * max(0.0f, dot(vertexToLightDir, viewNormal));
}

float3 GetSpecular(
    const in float3 diffuseColor, uniform float diffuseIntensity,
    const in float3 viewNormal, const in float3 vertexToLight, const in float3 viewPosition, const in float attResult, const in float specularPower)
{
    // ���� �ݻ�Ǵ� ������ ���� 
    const float3 reflectAngle = viewNormal * dot(vertexToLight, viewNormal); // �븻 ���Ϳ� ������Ʈ���� ���� ������ ����
    const float3 reflectVector = normalize(reflectAngle * 2.0f - vertexToLight); // ���� �ݻ�Ǵ� ������ ����
    
    // �ݻ籤 ��Ģ���� �ݻ籤�� ����
    return attResult * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-reflectVector), normalize(viewPosition))), specularPower);
}