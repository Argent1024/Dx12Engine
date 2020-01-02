#pragma once

#include "DXHelper.h"
#include "Shader.h"

namespace Graphic {
	class Primitive {
	public:
		Primitive(const Shader& pShader)
			: m_shader(pShader)
		{
		}

	protected:
		const Shader& m_shader;
	};


	class TriangleMesh : public Primitive {
	public:
		TriangleMesh(const Shader& pShader)
			: Primitive(pShader)
		{}

	};
	
}