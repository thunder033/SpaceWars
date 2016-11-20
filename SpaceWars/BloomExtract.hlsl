
cbuffer Data : register(b0)
{
	float BloomThreshold = 0.45f;
};

//Defines the input to this pixel shader
// - Should match the output of the postProcessVS
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
};

Texture2D ColorTexture	: register(t0);
SamplerState Sampler	: register(s0);

//Extract a glow map from the scene
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 color = ColorTexture.Sample(Sampler, input.uv);

	return saturate((color - BloomThreshold) / (1 - BloomThreshold));
}