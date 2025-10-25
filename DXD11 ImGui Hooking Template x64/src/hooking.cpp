#include <iostream>

#include "../include/hooking.h"
#include "../render/include/d3d11_vmt_indices.h"
#include "../render/include/rendercore.h"
#include "../memory/include/mem.h"
#include "../render/include/hackrender.h"
#include "../render/include/dx.h"
#include "../MinHook/include/MinHook.h"

HRESULT STDMETHODCALLTYPE Hooking::hookPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
{
	if (!d3d11.swapChain || !d3d11.device) {
		d3d11.initD3DDraw(swapChain);
	}

	if (d3d11.renderTarget == nullptr)
		d3d11.createRenderTarget();

	if (d3d11.renderTarget != nullptr)
		d3d11.deviceContext->OMSetRenderTargets(0, &d3d11.renderTarget, nullptr);

	if (!render.state.isImguiInit) {
		render.init(d3d11.window, d3d11.device, d3d11.deviceContext);

		render.state.isImguiInit = true;
	}

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		bool& showMenu = render.state.showMenu;
		showMenu = !showMenu;

		if (showMenu) {
			render.setHookWndProc(d3d11.window);
		}
		else {
			render.setWndProcToOriginal();
		}
	}

	if (render.state.showMenu) {
		render.loop(HackRender::render);
	}

	return d3d11.presentOriginal(swapChain, syncInterval, flags);
}

HRESULT STDMETHODCALLTYPE Hooking::hookResizeBuffer(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
{
	ImGui_ImplDX11_InvalidateDeviceObjects();
	HRESULT hr = d3d11.resizeBufferOriginal(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
	if (SUCCEEDED(hr)) {
		d3d11.createRenderTarget();
	}
	ImGui_ImplDX11_CreateDeviceObjects();
	return hr;
}

HRESULT STDMETHODCALLTYPE Hooking::hookCreateSwapChain(IDXGIFactory* pThis, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC pDesc, IDXGISwapChain** ppSwapChain)
{
	d3d11.renderTarget->Release();
	d3d11.renderTarget = NULL;

	HRESULT hr = d3d11.createSwapChainOriginal(pThis, pDevice, pDesc, ppSwapChain);

	if (SUCCEEDED(hr) && *ppSwapChain)
	{
		d3d11.swapChain = *ppSwapChain;
	}
	return hr;
}

bool Hooking::implementHooking()
{
	if (!d3d11.getDummyDeviceAndVtable()) {
		return false;
	}

	d3d11.present = (D3DX11::fnPresent)(d3d11.pVMT[(UINT)IDXGISwapChainVMT::Present]);
	d3d11.resizeBuffer = (D3DX11::fnResizeBuffer)(d3d11.pVMT[(UINT)IDXGISwapChainVMT::ResizeBuffers]);
	d3d11.createSwapChain = (D3DX11::fnCreateSwapChain)d3d11.factoryVMT[10];

	MH_Initialize();
	MH_CreateHook(d3d11.present, &hookPresent, reinterpret_cast<void**>(&d3d11.presentOriginal));
	MH_CreateHook(d3d11.resizeBuffer, &hookResizeBuffer, reinterpret_cast<void**>(&d3d11.resizeBufferOriginal));
	MH_CreateHook(d3d11.createSwapChain, &hookCreateSwapChain, reinterpret_cast<void**>(&d3d11.createSwapChainOriginal));
	MH_EnableHook(MH_ALL_HOOKS);

	return true;
}

bool Hooking::unHook()
{
	MH_DisableHook(d3d11.present);
	MH_DisableHook(d3d11.resizeBuffer);
	MH_DisableHook(d3d11.createSwapChain);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
	return true;
}

bool Hooking::freeCOM()
{
	if (d3d11.device) {
		d3d11.device->Release();
		d3d11.device = nullptr;
	}

	if (d3d11.deviceContext) {
		d3d11.deviceContext->Release();
		d3d11.deviceContext = nullptr;
	}

	if (d3d11.swapChain) {
		d3d11.swapChain->Release();
		d3d11.swapChain = nullptr;
	}

	if (d3d11.renderTarget) {
		d3d11.renderTarget->Release();
		d3d11.renderTarget = nullptr;
	}

	return true;
}

bool Hooking::freeGateway()
{
	//if (d3d11.present) {
	//	if (!VirtualFree(d3d11.present, 0, MEM_RELEASE)) return false;
	//}

	//if (d3d11.resizeBuffer) {
	//	if (!VirtualFree(d3d11.resizeBuffer, 0, MEM_RELEASE)) return false;
	//}

	return true;
}
