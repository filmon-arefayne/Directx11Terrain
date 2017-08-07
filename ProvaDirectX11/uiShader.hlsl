Texture2D diffuseMap : register(t0);
SamplerState textureSampler: register(s0);

cbuffer Matrici:register(b0)
{
	float4x4 ortho;
	float4x4 view;
	float4x4 world;
}

struct VertexInput
{
	float2 pos: POSITION;
	float2 tex: TEXCOORD;
};

struct PixelInput
{
	float4 pos: SV_POSITION;
	float2 tex: TEXCOORD;
};

PixelInput VSmain( VertexInput input )
{
	PixelInput output;

	output.pos = float4(input.pos,0,1);

	output.pos = mul(world, output.pos);
	output.pos = mul(view, output.pos);
	output.pos = mul(ortho, output.pos);

	output.tex = input.tex;

	return output;
}

float4 PSmain(PixelInput input) : SV_TARGET
{
	return diffuseMap.Sample(textureSampler, input.tex);
}