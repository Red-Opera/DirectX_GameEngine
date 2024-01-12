// WolrdViewProjection의 상수 버퍼
cbuffer ObjectWorldViewProjection
{
    float4 worldViewProjection;
};

void VertexShader(float3 position : POSITION, float4 color : COLOR, 
                  out float4 returnPosition : SV_Position, out float4 returnColor : COLOR)
{
    // 정점의 국소 공간을 동차 절단 공간으로 이동함
    returnPosition = mul(float4(position, 1.0f), worldViewProjection);
    
    // 정점 색상 그래도 다음 단계로 보냄
    returnColor = color;
}