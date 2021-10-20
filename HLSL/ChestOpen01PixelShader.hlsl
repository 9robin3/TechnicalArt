#include "../ParticleShaderStruct.hlsli"

PixelOutput main(GeometryToPixel input)
{
	
	float birthTime = myTimings.z;
	float deltaTime = myTimings.y;
	float timeDifference = deltaTime - birthTime;

	float2 uv = input.myUV.xy;
	float2 centerUv = uv - 0.5;
	float radialGradient = length(centerUv);
	
	
	float PI = 3.14159265f;
	float InitialLife = input.myInitialLifeTime;
	float SpentLife = input.myTimeSpentLiving / input.myInitialLifeTime;

	PixelOutput output;
	float myTexture = ParticleTexture.Sample(DefaultSampler, uv).r;
	float fade = saturate(myTexture * sin(SpentLife * PI) * (1.0 + InitialLife));
	//output.myColor = (textureColor.rgb, alpha) * input.myColor.rgba;
	//output.myColor = float4 (textureColor.rgb * input.myColor.rgb, alpha * input.myColor.a);
	//output.myColor = float4 (0.0,1.0,0.0, sin(SpentLife*PI)*(1.0+ InitialLife));
	
	
	
	float glowTime = pow(frac(birthTime * 1.5), 3);
	//float glowTime = pow(frac(birthTime * 0.7), 3));
	//float fadeOutTime = pow(frac(birthTime * 0.3 + 20), 3);
	float glow = smoothstep(0.2 - glowTime, 0.6 - glowTime, radialGradient);
	//float fadeOut = smoothstep(0.5, 0.0, birthTime * 0.3);*/

	float alpha = myTexture * glow * fade;
	float3 color = float3(1.0, 1.0, 1.0);

	output.myColor = float4(color,  alpha);
	//output.myColor = textureColor.rgba * input.myColor.rgba;
	return output;

	
}
