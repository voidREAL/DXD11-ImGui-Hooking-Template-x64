#pragma once
#include "../memory/include/hook.h"
#include "../render/include/dx.h"

class Hooking {
public:
	static HRESULT STDMETHODCALLTYPE hookPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
	static HRESULT STDMETHODCALLTYPE hookResizeBuffer(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags); //Handle device / resource reset
	static HRESULT STDMETHODCALLTYPE hookCreateSwapChain(IDXGIFactory* pThis, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC pDesc, IDXGISwapChain** ppSwapChain);

	static bool implementHooking();
	static bool unHook();
	static bool freeCOM();
	static bool freeGateway();
};