Texture2D diffuseMap : register(t0);
SamplerState textureSampler: register(s0);

cbuffer Luci:register(b0)
{
	float4 AmbientColor;
	float4 lightColor;
	float4 lightDirection;
	float4 SpecularColor;
	float SpecularPower;
}


cbuffer Matrici:register(b1)
{
	float4x4 projection;
	float4x4 view;
	float4x4 world;
	float3 cameraPosition;

}
struct vsInput
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 color : COLOR;
};

struct psInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float3 lightDirection : TEXCOORD1;
	float3 viewDirection : TEXCOORD2;

};

/************* Vertex Shader *************/
psInput VSmain(vsInput input)
{
	psInput output;
	
	output.pos = float4(input.pos, 1);

	output.pos = mul(world, output.pos);
	output.pos = mul(view, output.pos);
	output.pos = mul(projection, output.pos);

	output.color = float4(input.color,1.0f);
	output.tex = input.tex;

	// NORMALE

	output.normal = normalize(mul(float4(input.normal, 0), world).xyz);
	output.lightDirection = normalize(-lightDirection.xyz);

	// view direction

	float3 worldPosition = mul(input.pos, world).xyz;

	output.viewDirection = normalize(cameraPosition - worldPosition);
	return output;
}


/************* Pixel Shader *************/


float4 PSmain(psInput input) : SV_TARGET
{
	//return diffuseMap.Sample(textureSampler,input.tex);
	//return input.color;
	
	// AMBIENT COLOR 
	if (input.color.r== 0 && input.color.g == 0 && input.color.b == 0)
	{
		input.color = 1.0f;
	}
	float4 color = diffuseMap.Sample(textureSampler,input.tex)*input.color;

	float3 ambient = AmbientColor.rgb * AmbientColor.a * color.rgb; // Color (.rgb) * Intensity (.a)

	// DIFFUSE LIGHTING

	float3 normal = normalize(input.normal);  // il rasterized stage puo modificare i dati rendendoli non-unit-lenght vector
	float3 LightDirection = normalize(input.lightDirection);
	float3 n_dot_l = dot(LightDirection, normal);
	float3 viewDirection = normalize(input.viewDirection);

	float3 diffuse = (float3)0;
	float3 specular = (float3)0;

	if (n_dot_l.r > 0 && n_dot_l.g > 0 && n_dot_l.b > 0)   // se è uguale a 0.0 vuol dire che il lightvector è parallelo alla superficie nessun effetto , invece se è 1.0 vuol dire che è perpendicolare max effetto
	{
		diffuse = lightColor.rgb * lightColor.a * n_dot_l * color.rgb;

		// R = 2 * (N.L) * N - L
		float3 reflectionVector = normalize(2 * n_dot_l * normal - LightDirection);

		// specular = R.V^n with specular map stored in color texture's alpha channel
		specular = SpecularColor.rgb * SpecularColor.a * min(pow(saturate(dot(reflectionVector, viewDirection)),SpecularPower), color.w);
	}

	float4 output = (float4)0;
	
	output.rgb = ambient + diffuse +specular;
	output.a = 1.0f;



	return output;
}

[maxvertexcount(72)]
void GSmain(triangle psInput gIn[3],inout TriangleStream<psInput> TriStream )
{
	
}