// Read a spherical coordinate image, turn it into a cube map

#define PI 3.141592653589793f
#define TwoPI 6.283185307179586f

cbuffer InfoBuffer : register(b0) 
{
    float InvResolution;
};

Texture2D<float4> EnvMap : register(t0);
RWTexture2DArray<float4> CubeMap : register(u0);

SamplerState BilinearSampler : register(s0);


float2 Dir2Sphere(float3 coor) {
    float theta = acos(coor.z);
    float sinT = sin(theta);
    float phi = atan2(coor.y / sinT, coor.x / sinT);
    return float2(phi / PI, theta / PI);
}


[numthreads( 8, 8, 1 )]
void CSMain(uint3 DispatchThreadID : SV_DispatchThreadID )
{   
    uint2 cube_coor = DispatchThreadID.xy;
    float2 uv =  DispatchThreadID.xy * InvResolution;
    float3 pixel_pos;   
    float2 sphere_coord;
    float4 Color;

    
    // 0: +X
    pixel_pos = normalize(float3(1.0, uv));
    sphere_coord = Dir2Sphere(pixel_pos);
    Color = EnvMap.SampleLevel(BilinearSampler, sphere_coord, 0);
    CubeMap[uint3 (cube_coor, 0)] = Color;

    // 1: -X
    pixel_pos = normalize(float3(-1.0, uv));
    sphere_coord = Dir2Sphere(pixel_pos);
    Color = EnvMap.SampleLevel(BilinearSampler, sphere_coord, 0);
    CubeMap[uint3 (cube_coor, 1)] = Color;

    // 2: +Y
    pixel_pos = normalize(float3(uv.x, 1.0, uv.y));
    sphere_coord = Dir2Sphere(pixel_pos);
    Color = EnvMap.SampleLevel(BilinearSampler, sphere_coord, 0);
    CubeMap[uint3 (cube_coor, 2)] = Color;

    // 3: -Y
    pixel_pos = normalize(float3(uv.x, -1.0, uv.y));
    sphere_coord = Dir2Sphere(pixel_pos);
    Color = EnvMap.SampleLevel(BilinearSampler, sphere_coord, 0);
    CubeMap[uint3 (cube_coor, 3)] = Color;

    // 4: +Z
    pixel_pos = normalize(float3(uv, 1.0));
    sphere_coord = Dir2Sphere(pixel_pos);
    Color = EnvMap.SampleLevel(BilinearSampler, sphere_coord, 0);
    CubeMap[uint3 (cube_coor, 4)] = Color;

    // 5: -Z
    pixel_pos = normalize(float3(uv, -1.0));
    sphere_coord = Dir2Sphere(pixel_pos);
    Color = EnvMap.SampleLevel(BilinearSampler, sphere_coord, 0);
    CubeMap[uint3 (cube_coor, 5)] = Color;
}