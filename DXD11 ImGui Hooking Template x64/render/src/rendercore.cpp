#include "../include/rendercore.h"
#include "../include/dx.h"

Render render;
LONG_PTR Render::originalWndProc = NULL;
HWND Render::window = NULL;

bool Render::init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	state.isImguiInit = false;
	state.showMenu = false;

	ImGui_ImplWin32_EnableDpiAwareness();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device, deviceContext);

	return true;
}

bool Render::loop(void(*renderCallback)())
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	renderCallback();

	ImGui::EndFrame();
	ImGui::Render();
	d3d11.deviceContext->OMSetRenderTargets(1, &d3d11.renderTarget, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return true;
}

bool Render::cleanup()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return true;
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall Render::WndProcHook(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (Render::handleImGuiInput(hWnd, uMsg, wParam, lParam)) {
		return true;
	}

	return CallWindowProc((WNDPROC)originalWndProc, hWnd, uMsg, wParam, lParam);
}

bool Render::handleImGuiInput(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!render.state.isImguiInit) {
		return CallWindowProc((WNDPROC)originalWndProc, hWnd, uMsg, wParam, lParam);
	}

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return true;
	}

	ImGuiIO& io = ImGui::GetIO();
	if (render.state.showMenu && (io.WantCaptureMouse || io.WantCaptureKeyboard))
		return true;
}

bool Render::setHookWndProc(HWND handleWindow, LONG_PTR wndProc /*= (LONG_PTR)WndProcHook*/)
{
	if (!handleWindow) {
		return false;
	}
	window = handleWindow;
	originalWndProc = SetWindowLongPtr(window, GWLP_WNDPROC, wndProc);
	return true;
}

bool Render::setWndProcToOriginal()
{
	if (window && originalWndProc) {
		SetWindowLongPtr(window, GWLP_WNDPROC, originalWndProc);
		return true;
	}
	else {
		return false;
	}
}
