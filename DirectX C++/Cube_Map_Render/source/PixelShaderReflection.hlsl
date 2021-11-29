
cbuffer CameraBuffer : register(b1)
{
    float4 light_pos;
    float4 camera_pos;
}

struct PSIn
{
    float4 pos : SV_Position;
    float3 worldPos : WORLDPOS;
    float3 normal : NORMAL;
};

TextureCube tex : register(t5);
sampler samp : register(s0);

float4 main(PSIn input) : SV_TARGET
{
    float3 texCord;
    float3 V = normalize(input.worldPos - camera_pos.xyz);
    texCord = reflect(V, input.normal);
    return tex.Sample(samp, texCord);
}