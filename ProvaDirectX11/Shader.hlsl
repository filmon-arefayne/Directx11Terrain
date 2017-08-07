Texture2D diffuseMap : register(t0);
SamplerState textureSampler: register(s0);

cbuffer Matrici:register(b1)
{
	float4x4 projection;
	float4x4 view;
	float4x4 world;
}
struct vsInput
{
	float3 pos : POSITION;
	//float4 color : COLOR;
	float2 tex : TEXCOORD;
};

struct psInput
{
	float4 pos : SV_POSITION;
	//float4 color : COLOR;
	float2 tex : TEXCOORD;
};



psInput VSmain(vsInput input)
{
	psInput output;

	output.pos = float4(input.pos, 1);

	output.pos = mul(world, output.pos);
	output.pos = mul(view, output.pos);
	output.pos = mul(projection, output.pos);

	//output.color = input.color;

	output.tex = input.tex;

	return output;
}
float4 PSmain(psInput input) : SV_TARGET
{
	return diffuseMap.Sample(textureSampler,input.tex);
	//return input.color;

}