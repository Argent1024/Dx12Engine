#pragma once
#include <iostream>
#include "DXCore.h"
#include "Shader.h"

namespace Graphic 
{

	class GraphicCore {
	public:
		GraphicCore(UINT t_width, UINT t_height, LPCTSTR t_title=L"playground"):
			m_width(t_width), m_height(t_height), m_title(t_title){
			m_dxCore = new DXCore(t_width, t_height);
			
		}
		UINT GetWidth() { return m_width; }
		UINT GetHeight() {return m_height; }
		LPCTSTR GetTitle() { return m_title; }

		void Init(const HWND m_appHwnd) {
			m_dxCore->Init(m_appHwnd);
			std::wstring path = L"D:\\work\\tEngine\\src\\Shaders\\shaders.hlsl";
			shader = new Shader(*m_dxCore, L"test", path, path);
		}

		void Render() {
			shader->Render();
			m_dxCore->WaitFrame();
		}

	private:
		const UINT m_width;
		const UINT m_height;
		const LPCTSTR m_title;


		DXCore* m_dxCore;
		Shader* shader;
	};
}