float PS(float4 position : SV_Position, float4 viewPosition : Position) : SV_TARGET
{
    float bias = 0.0001 + (position.z - viewPosition.w / position.w) * 0.0001;
    
    return saturate(length(viewPosition.xyz) / 100.0f + bias);
}
