#pragma once

#include "GObject.h"

namespace Game {
	class Scene 
	{
	public:
		virtual void Render();

		virtual void Update();

	protected:
		std::vector<ptrGobject> m_ObjList;
	};

}