#pragma once
#include "../include.h"
#define SAFE_RELEASE(x) if (x) {x->Release(); x = nullptr;}

class D3DX11 {
public:
	ID3D11Device* device = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ID3D11RenderTargetView* renderTarget = nullptr;

	void** pVMT[17];
	void** factoryVMT[20];
	HWND window;

	using fnPresent = HRESULT(__stdcall*)(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
	using fnResizeBuffer = HRESULT(__stdcall*)(IDXGISwapChain* swapChain, INT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
	using fnCreateSwapChain = HRESULT(__stdcall*)(IDXGIFactory* pThis, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC pDesc, IDXGISwapChain** ppSwapChain);

	fnPresent present;
	fnPresent presentOriginal;
	fnResizeBuffer resizeBuffer;
	fnResizeBuffer resizeBufferOriginal;
	fnCreateSwapChain createSwapChain;
	fnCreateSwapChain createSwapChainOriginal;

	bool getDummyDeviceAndVtable();
	bool getDeviceContextRenderTarget();
	bool createRenderTarget();
	HWND getHandleWindow();
	bool initD3DDraw(IDXGISwapChain* pSwapChain);
};

extern D3DX11 d3d11;