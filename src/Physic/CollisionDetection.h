#pragma once

#include "Math/Volume.h"
#include "Octree.h"
#include "PhysicState.h"

namespace Physic 
{

	class CollisionDection {

	public:
		void CollisionUpdate()
		{
			size_t s = m_CollisionObjects.SizeAll();
			for (int i = 0; i < s; ++s) {
				const std::vector<ptrCpstate>& volumes = m_CollisionObjects.Voxel(i);


			}
		}


	private:
		UniformGrid<ptrCpstate> m_CollisionObjects;
	};


}