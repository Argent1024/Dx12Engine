#pragma once

#include "CommandList.h"
#include "GObject.h"
#include "Camera.h"
#include "Light.h"

namespace Game 
{

	class Scene 
	{
	public:
		virtual void Render();

		virtual void Update();

	protected:
		// TODO need change
		Camera m_Camera;
		std::vector<ptrLight> m_LightList;
		std::vector<ptrGobject> m_ObjList;
	};

}