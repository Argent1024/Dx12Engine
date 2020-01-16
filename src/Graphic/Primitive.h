#pragma once

#include "DXHelper.h"

namespace Graphic {
	/*
		Store mesh data used in Shader
	*/
	class Primitive {
	public:
		Primitive()
		{
		}
	};


	class TriangleMesh : public Primitive {
	public:
		TriangleMesh()
			: Primitive()
		{}

	};
	
}