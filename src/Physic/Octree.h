#pragma once
#include "stdafx.h"
#include "Math/MathLib.h"



template <int dim, class T>
class Octree {
public:
	constexpr UINT8 NumChildren = 1 << dim;
	using VectorD = typename Vector<dim>::Type;



};


template <class T>
class UniformGrid {
public:
	UniformGrid(Math::Vector3 center = Math::Vector3(Math::kZero),
		Math::Vector3 scale = Math::Vector3(1, 1, 1),
		UINT x = 10, UINT y = 10, UINT z = 10) 
		: resx(x), resy(y), resz(z)
	{
		MaxSize = resx * resy * resz;
		m_Voxels(MaxSize, { });
	}

	std::vector<T>& Voxel(UINT x, UINT y, UINT z) 
	{ return m_Voxels[toIndex(x,y,z)]; }

	const std::vector<T>& Voxel(UINT x, UINT y, UINT z) const
	{ return m_Voxels[toIndex(x,y,z)]; }

	std::vector<T>& Voxel(UINT i) 
	{ return m_Voxels[i]; }

	const std::vector<T>& Voxel(UINT i) const
	{ return m_Voxels[i]; }

	UINT SizeX() const { return resX; }
	UINT SizeY() const { return resY; }
	UINT SizeZ() const { return resZ; }

	size_t SizeAll() const { return MaxSize; }

private:

	inline size_t toIndex(UINT x, UINT y, UINT z) 
	{
		assert(x < resx && y < resy && z < resz);
		return x * resy * resz + y * resz + z;
	}

	UINT resx, resy, resz;
	size_t MaxSize;

	std::vector<std::vector<T>> m_Voxels;

};