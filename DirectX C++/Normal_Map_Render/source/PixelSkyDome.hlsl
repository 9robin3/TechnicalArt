struct PSIn
{
    float4 pos : SV_Position;
    float3 texCord : TEXCOORD0;
};

TextureCube tex : register(t5);
sampler samp : register(s0);

float4 main(PSIn input) : SV_TARGET
{
    return tex.Sample(samp, input.texCord);
}