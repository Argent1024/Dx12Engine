// Read a spherical coordinate image, turn it into a cube map

#define PI 3.141592653589793f
#define TwoPI 6.283185307179586f


// EnvMap is also Mip 0 for roughness 0
TextureCube<float4> EnvMap : register(t0);
RWTexture2D<float4> CubeMap : register(u0);

