cbuffer ConstBuffer	: register(b0)
{
	matrix matWorld;
	matrix matView;
	matrix matProj;
	float4 diffuse;
};


struct VS_INPUT
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(float4(input.Pos,1),matWorld);
	output.Pos = mul(output.Pos, matView);
	output.Pos = mul(output.Pos, matProj);

	output.Tex = input.Tex;
	return output;
}