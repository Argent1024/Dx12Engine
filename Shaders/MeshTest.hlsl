cbuffer Transformation : register(b0) 
{
	float4x4 mvp;
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
	
    result.position = mul(mvp, input.position);
	result.normal = input.normal;
    result.uv = input.uv;
    return result;
}


float4 PSMain(PSInput input) : SV_TARGET
{
	// return input.position.z;

	// return float4(input.normal, 1);
	
	float4 lightDir = {1, 1, 0, 0};
	return dot(input.normal, lightDir) * 0.9 + 0.05;
}