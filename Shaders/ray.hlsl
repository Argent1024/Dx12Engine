cbuffer ScreenData : register(b0)
{
    float4 ScreenSize;
};

struct VSInput
{
	float4 position: POSITION;
	float color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput result;

    result.position = input.position;
    result.color = input.color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
   float2 coor = (input.position / ScreenSize).xy;
   return coor.x;
}

