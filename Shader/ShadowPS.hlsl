float PS(float4 position : SV_Position, float4 viewPosition : Position) : SV_TARGET
{
    float bias = 0.0005 + (position.z - viewPosition.w / position.w);
    
    return saturate(length(viewPosition.xyz) / 100.0f + bias);
}