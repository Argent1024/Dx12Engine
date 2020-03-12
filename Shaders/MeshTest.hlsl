cbuffer Transformation : register(b0) 
{
	float4x4 ViewProjective;
};

struct VSInput 
{
	float4 position : POSITION0;
	float3 normal   : NORMAL0;
	float2 uv		: TEXCOORD0;

};

struct PSInput
{
    float4 position : SV_POSITION;
	float3 normal   : COLOR0;
    float2 uv		: TEXCOORD0;
};


// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput result;
	
    result.position = mul(ViewProjective, input.position);
	result.normal = input.normal;
    result.uv = input.uv;
    return result;
}


float4 PSMain(PSInput input) : SV_TARGET
{
	return 1.0f;//float4(input.normal, 1);
}