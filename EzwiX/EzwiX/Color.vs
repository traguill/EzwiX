
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float color : COLOR;
};

PixelInputType ColorVertexShader(VertexInputType in)
{
	PixelInputType out;

	in.position.w = 1.0f;

	out.position = mul(in.position, worldMatrix);
	out.position = mul(in.position, viewMatrix);
	out.position = mul(in.position, projectionMatrix);

	out.color = in.color;

	return out;
}

