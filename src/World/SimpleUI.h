#pragma once

#include "Graphic/PipelineState.h"
#include "Graphic/RootSignature.h"
#include "Graphic/CommandList.h"

#include "ext/imgui/imgui.h"
#include "ext/imgui/backends/imgui_impl_win32.h"
#include "ext/imgui/backends/imgui_impl_dx12.h"

namespace Engine {
	class UiEngine {
	private:
		bool show_demo_window = true;

	public:
		virtual void Initialize(const HWND appHwnd) {
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			
			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();

			// Setup Platform/Renderer backends
			Graphic::DescriptorHeap* uiheap = Engine::GetUIHeap();
			ImGui_ImplWin32_Init(appHwnd);
			ImGui_ImplDX12_Init(Engine::GetDevice(), 2,
				DXGI_FORMAT_R8G8B8A8_UNORM, uiheap->GetDescriptorHeap(),
				uiheap->GetCPUHandle(0),
				uiheap->GetGPUHandle(0));
		}

		virtual void Render(Graphic::CommandList& ctx) {
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);

			UpdateUIWindows();
			
			// Rendering
			ImGui::Render();

			// Render using graphics
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), ctx.GetCommandList());
			
		}

		virtual void UpdateUIWindows() = 0;
	};


	class SimpleUIEngine : public UiEngine {
		void UpdateUIWindows() override { }
	};

}

//namespace UI {
//	/***********************  Some simple ui uitility   ************************/
//	void ColorEdit3(const std::string& name, DirectX::XMFLOAT3& col) {
//		 ImGui::ColorEdit3(name.c_str(), (float*)&col); 
//	}
//
//	void SliderFloat(const std::string& name, FLOAT& f, const FLOAT m=0.0f, const FLOAT M=1.0f) {
//		ImGui::SliderFloat("float", &f, m, M);    
//	}
//}

using ptrUiEngine = std::shared_ptr<Engine::UiEngine>;