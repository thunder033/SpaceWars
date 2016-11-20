cbuffer Data : register(b0)
{
	float pixelWidth;
	float pixelHeight;
	int blurAmount;
}

//Defines the input to this pixel shader
// - Should match the output of the postProcessVS
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
};

Texture2D Pixels		: register(t0);
SamplerState Sampler	: register(s0);

//NOT ACTUALLY GAUSSION BLUR (YET)
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 color = float4(0,0,0,0);
	uint numSamples = 0;

	for (int y = -blurAmount; y < blurAmount; y++)
	{
		for (int x = -blurAmount; x < blurAmount; x++)
		{
			//Calculate uv of this pixel
			float2 uv = input.uv + float2(x * pixelWidth, y * pixelWidth);

			//Sample and add
			color += Pixels.Sample(Sampler, uv);
			numSamples++;
		}
	}

	return color / numSamples;
}