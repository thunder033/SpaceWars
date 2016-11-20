static const float3 GrayScaleIntensity = { 0.299f, 0.587f, 0.114f };

cbuffer Data : register(b0)
{
	float BloomIntensity = 1.25f;
	float BloomSaturation = 1.0f;
	float SceneIntensity = 1.0f;
	float SceneSaturation = 1.0f;
};

/************* Data Structures **************/

//Defines the input to this pixel shader
// - Should match the output of the postProcessVS
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
};

Texture2D ColorTexture	: register(t0);
Texture2D BloomTexture  : register(t1);

SamplerState Sampler	: register(s0);

/************* Utility Functions **************/

float4 AdjustSaturation(float4 color, float saturation)
{
	float intensity = dot(color.rgb, GrayScaleIntensity);

	return float4(lerp(intensity.rrr, color.rgb, saturation), color.a);
}

//Composites the (blurred) glow map with the original scene
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 sceneColor = ColorTexture.Sample(Sampler, input.uv);
	float4 bloomColor = BloomTexture.Sample(Sampler, input.uv);

	sceneColor = AdjustSaturation(sceneColor, SceneSaturation) * SceneIntensity;
	bloomColor = AdjustSaturation(bloomColor, SceneSaturation) * BloomIntensity;

	sceneColor *= (1 - saturate(bloomColor));

	return sceneColor + bloomColor;
}