Texture2D texDiffuse : register(t0);
Texture2D texNormal : register(t1);
SamplerState texSampler : register(s0);


struct PSIn
{
    float4 Pos : SV_Position;
    float3 WorldPos : POS;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEX;

};

cbuffer MaterialBuffer : register(b0)
{
    float4 Ka, Kd, Ks;
}

cbuffer CameraBuffer : register(b1)
{
    float4 light_pos;
    float4 camera_pos;
	//float2 padding;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 main(PSIn input) : SV_Target
{
    float3 T = normalize(input.Tangent);
    float3 B = normalize(input.Binormal);
    float3 N = normalize(input.Normal);
    
    float3 texNormalColor = texNormal.Sample(texSampler, input.TexCoord) * 2 - 1;

    float3x3 TBN = transpose(float3x3(T, B, N));
	//float3 TBN_N = TBN * N;
    float3 TBN_N = mul(TBN, texNormalColor);

    float4 texDiffuseColor = texDiffuse.Sample(texSampler, input.TexCoord);

    float3 L = normalize(light_pos - input.WorldPos);
    float3 V = normalize(camera_pos - input.WorldPos);
    float3 R = reflect(-L, TBN_N);
    float a = 500;

    float4 Id = saturate(texDiffuseColor * dot(TBN_N, L));
    float4 Is = saturate(Ks * pow(dot(R, V), a));
    float4 I = Ka + Id + Is;

    return I;
    //return float4(input.Normal * 0.5f + 0.5f, 1);
}