cbuffer LightInfoConstant
{
    float3 lightPosition;
    float3 ambient;         // 주변광 강도
    float3 diffuseColor;    // 분산광 색상
    float diffuseIntensity; // 분산광 강도
    float attConst;         // 빛 감쇠 상수
    float attLin;           // 빛 강쇠 선형 감소
    float attQuad;          // 빛 상쇠 제곱 감소
};             

cbuffer ObjectColor
{
    float3 materialColors[6];   // 물체의 재질 색상
    float padding;
    float specularIntensity;    // Specular 세기
    float specularPower;        // Specular 강도
};

float4 PS(float3 worldPosition : Position, float3 normal : Normal, uint triangleID : SV_PrimitiveID) : SV_Target
{
    // 정점에서 빛의 거리와 방향을 구함
    const float3 vertexToLight = lightPosition - worldPosition;
    const float distance = length(vertexToLight);
    const float3 vertexToLightDir = vertexToLight / distance;
    
    // 상쇠된 빛의 강도를 구하고 분산광을 거리에 따른 빛의 강도를 구함
    const float attResult = 1.0f / (attConst + attLin * distance + attQuad * (distance * distance));
    const float3 diffuse = diffuseColor * diffuseIntensity * attResult * max(0.0f, dot(vertexToLightDir, normal));
    
    // 빛이 반사되는 방향을 구함 
    const float3 reflectAngle = normal * dot(vertexToLight, normal);    // 노말 벡터와 오브젝트에서 빛의 각도를 구함
    const float3 reflectVector = reflectAngle * 2.0f - vertexToLight;   // 빛이 반사되는 각도를 구함
    
    // 반사광 법칙으로 반사광을 구함
    const float3 specular = attResult * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-reflectVector), normalize(worldPosition))), specularPower);
    
    // 주변광과 분산광을 합친 후 1.0이 넘어간 경우 최대 1.0으로 제한함
    return float4(saturate(diffuse + ambient + specular) * materialColors[(triangleID / 2) % 6],
    1.0f);
}