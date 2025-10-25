#include "../include/dx.h"
#include "../include/d3d11_vmt_indices.h"
#include <iostream>

D3DX11 d3d11;

bool D3DX11::getDummyDeviceAndVtable()
{
	DXGI_SWAP_CHAIN_DESC sd;

	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
	sd.BufferCount = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.OutputWindow = GetDesktopWindow();
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 1;

	ID3D11Device* tmpDevice = nullptr;
	IDXGISwapChain* tmpSwapChain = nullptr;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &tmpSwapChain, &tmpDevice, nullptr, nullptr);
	if (GetLastError() == 0x594) {
		SetLastError(0);
	}

	if (FAILED(hr)) {
		return false;
	}

	IDXGIFactory* factory = nullptr;
	
	//Get VMT of factory
	{
		IDXGIDevice* pDXGIDevice = nullptr;
		tmpDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);

		IDXGIAdapter* pAdapter = nullptr;
		pDXGIDevice->GetAdapter(&pAdapter);
		pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);
		memcpy(factoryVMT, *(void***)factory, sizeof(factoryVMT));

		SAFE_RELEASE(pAdapter);
		SAFE_RELEASE(pDXGIDevice);
		SAFE_RELEASE(factory);
	}
	
	//Get VMT of swapchain
	{
		void** tempVMT = *(void***)tmpSwapChain;
		memcpy(pVMT, tempVMT, sizeof(pVMT));

		SAFE_RELEASE(tmpDevice);
		SAFE_RELEASE(tmpSwapChain);
	}

	return true;
}

bool D3DX11::getDeviceContextRenderTarget()
{
	HRESULT hr = swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device);
	if (FAILED(hr)) {
		return false;
	}

	window = getHandleWindow();

	device->GetImmediateContext(&deviceContext);
	if (!createRenderTarget()) {
		return false;
	}

	return true;
}

bool D3DX11::createRenderTarget()
{
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr)) {
		return false;
	}

	hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTarget);
	backBuffer->Release();
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

HWND D3DX11::getHandleWindow()
{
	if (!swapChain) {
		return nullptr;
	}

	DXGI_SWAP_CHAIN_DESC sd;
	HRESULT hr = swapChain->GetDesc(&sd);
	if (FAILED(hr)) {
		return nullptr;
	}
	return sd.OutputWindow;
}

bool D3DX11::initD3DDraw(IDXGISwapChain* pSwapChain)
{
	swapChain = pSwapChain;
	return getDeviceContextRenderTarget();
}
