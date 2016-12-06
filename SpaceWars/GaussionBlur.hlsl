cbuffer Data : register(b0)
{
	float pixelWidth;
	float pixelHeight;
	float2 dir;
}

static const float offset[] = { 0.0, 1.0, 2.0, 3.0, 4.0 };
static const float weights[] = { 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 };

//Defines the input to this pixel shader
// - Should match the output of the postProcessVS
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
};

Texture2D Pixels		: register(t0);
SamplerState Sampler	: register(s0);

// Guassion Blur
// http://stackoverflow.com/questions/36303950/hlsl-gaussian-blur-effect
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 color = Pixels.Sample(Sampler, input.uv) * weights[0];
	float4 fragmentColor = float4(0, 0, 0, 0);

	float hstep = dir.x;
	float vstep = dir.y;

	for (int i = 1; i < 5; i++)
	{
		float2 uvOffset = float2(hstep * offset[i] * pixelWidth, vstep * offset[i] * pixelHeight);
		fragmentColor +=
			Pixels.Sample(Sampler, input.uv + uvOffset) * weights[i] +
			Pixels.Sample(Sampler, input.uv - uvOffset) * weights[i];
	}

	color += fragmentColor;
	return color;
}