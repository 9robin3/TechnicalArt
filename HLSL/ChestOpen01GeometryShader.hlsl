#include "../ParticleShaderStruct.hlsli"

[maxvertexcount(4)]
void main(
	point VertexToGeometry input[1],
	inout TriangleStream< GeometryToPixel > output
)
{
	const float2 offset[4] =
	{
		{-0.5f, 0.5f},
		{0.5f, 0.5f},
		{-0.5f, -0.5f},
		{0.5f, -0.5f}
	};
	const float2 uv[4] =
	{
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
	};

	VertexToGeometry inputvertex = input[0];
	for (unsigned int index = 0; index < 4; index++)
	{
		GeometryToPixel vertex;
		vertex.myPosition = inputvertex.myPosition;

		float4 transformedoffset = float4(offset[index] * inputvertex.mySize, 0, 0);
		float4 placeholderoffset = float4(0, 0, 0, 0);

		placeholderoffset.x = transformedoffset.x * cos(inputvertex.myRotation.z) - transformedoffset.y * sin(inputvertex.myRotation.z);
		placeholderoffset.y = transformedoffset.x * sin(inputvertex.myRotation.z) + transformedoffset.y * cos(inputvertex.myRotation.z);
		transformedoffset = placeholderoffset;

		placeholderoffset.z = transformedoffset.z * cos(inputvertex.myRotation.x) + transformedoffset.y * sin(inputvertex.myRotation.x);
		placeholderoffset.y = -transformedoffset.z * sin(inputvertex.myRotation.x) + transformedoffset.y * cos(inputvertex.myRotation.x);
		transformedoffset = placeholderoffset;

		placeholderoffset.x = transformedoffset.x * cos(inputvertex.myRotation.y) + transformedoffset.z * sin(inputvertex.myRotation.y);
		placeholderoffset.z = -transformedoffset.x * sin(inputvertex.myRotation.y) + transformedoffset.z * cos(inputvertex.myRotation.y);
		transformedoffset = placeholderoffset;

		float tau = 6.283f;
		float ninetydegreeangle = tau / 4.f;
		placeholderoffset.z = transformedoffset.z * cos(ninetydegreeangle) + transformedoffset.y * sin(ninetydegreeangle);
		placeholderoffset.y = -transformedoffset.z * sin(ninetydegreeangle) + transformedoffset.y * cos(ninetydegreeangle);
		transformedoffset = placeholderoffset;

		transformedoffset = mul(toCamera, transformedoffset);

		vertex.myPosition.xyz += transformedoffset.xyz;
		vertex.myPosition = mul(toProjection, vertex.myPosition);
		vertex.myColor = inputvertex.myColor;
		vertex.myUV = uv[index];
		vertex.myInitialLifeTime = inputvertex.myInitialLifeTime;
		vertex.myTimeSpentLiving = inputvertex.myTimeSpentLiving;
		vertex.myUniqueID = inputvertex.myUniqueID;
		output.Append(vertex);
	}
}




