cbuffer cb0
{
    float4 ScreenSize;
};


struct PSInput
{
    float4 position : SV_POSITION; //[0, width] * [0, height]
	float4 color : COLOR;
};


struct VSInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};


PSInput VSMain(VSInput v)
{
    PSInput result;

    result.position = v.position ;
	result.color = v.color;
   // result.size = ScreenSize.xy;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}
