#include "Graphic/PipelineState.h"
#include "Graphic/RootSignature.h"
#include "Graphic/CommandList.h"

#include "ext/imgui/imgui.h"
#include "ext/imgui/backends/imgui_impl_win32.h"
#include "ext/imgui/backends/imgui_impl_dx12.h"


namespace Engine {
	class UiEngine {
	public:
		static void Initialize(const HWND appHwnd) {
			show_demo_window = true;

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
		
		static void Render(Graphic::CommandList& ctx) {
			ImGuiIO& io = ImGui::GetIO(); (void)io;

			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);

			{
				f = 0.0f;
				counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				// ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}


			// Rendering
			ImGui::Render();

			// Render using graphics
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), ctx.GetCommandList());
			
			// TODO fence stuff
		}

		static bool show_demo_window;
		static float f;
		static int counter;

	};


}