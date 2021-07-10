// A simple copy of https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/GenerateMipsCS.hlsli

RWTexture2D<float4> Mip1 : register(u0, space1);
RWTexture2D<float4> Mip2 : register(u1, space1);
RWTexture2D<float4> Mip3 : register(u2, space1);
RWTexture2D<float4> Mip4 : register(u3, space1);


Texture2D<float4> SrcMip : register(t0, space1);

SamplerState BilinearSampler : register(s0);


groupshared float gs_R[64];
groupshared float gs_G[64];
groupshared float gs_B[64];
groupshared float gs_A[64];

cbuffer InfoBuffer : register(b2) 
{
    uint SrcMipLevel;	// Texture level of source mip
    uint NumMipLevels;	// Number of OutMips to write: [1, 4]
    float2 TexelSize;	// 1.0 / OutMip1.Dimensions
};

float4 LoadColor( uint Index )
{
    return float4( gs_R[Index], gs_G[Index], gs_B[Index], gs_A[Index]);
}

void StoreColor( uint Index, float4 Color )
{
    gs_R[Index] = Color.r;
    gs_G[Index] = Color.g;
    gs_B[Index] = Color.b;
    gs_A[Index] = Color.a;
}


[numthreads( 8, 8, 1 )]
void CSMain( uint GI : SV_GroupIndex, uint3 DispatchThreadID : SV_DispatchThreadID )
{
    // Assume the size of input is power of two
    float2 UV = TexelSize * (DispatchThreadID.xy + 0.5);
    float4 Src1 = float4(1.0, 0.0, 0.0, 0.0);//SrcMip.SampleLevel(BilinearSampler, UV, SrcMipLevel);


    Mip1[DispatchThreadID.xy] = Src1;
    if(NumMipLevels == 1) 
        return;

    StoreColor(GI, Src1);
    GroupMemoryBarrierWithGroupSync();

    // checks x & y are even
    if ((GI & 0x9) == 0)
    {
        float4 Src2 = LoadColor(GI + 0x01);
        float4 Src3 = LoadColor(GI + 0x08);
        float4 Src4 = LoadColor(GI + 0x09);
        Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

        Mip2[DispatchThreadID.xy / 2] = Src1;
        StoreColor(GI, Src1);
    }

    if (NumMipLevels == 2)
        return;

    // This bit mask (binary: 011011) checks that X and Y are multiples of four.
    if ((GI & 0x1B) == 0)
    {
        float4 Src2 = LoadColor(GI + 0x02);
        float4 Src3 = LoadColor(GI + 0x10);
        float4 Src4 = LoadColor(GI + 0x12);
        Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

        Mip3[DispatchThreadID.xy / 4] = Src1;
        StoreColor(GI, Src1);
    }

    if (NumMipLevels == 3)
        return;

    GroupMemoryBarrierWithGroupSync();

    // This bit mask would be 111111 (X & Y multiples of 8), but only one
    // thread fits that criteria.
    if (GI == 0)
    {
        float4 Src2 = LoadColor(GI + 0x04);
        float4 Src3 = LoadColor(GI + 0x20);
        float4 Src4 = LoadColor(GI + 0x24);
        Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

        Mip4[DispatchThreadID.xy / 8] = Src1;
    }
}