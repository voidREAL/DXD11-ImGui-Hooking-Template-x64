#pragma once
#include <Windows.h>
#include "../include.h"

class Render {
public:
	struct State {
		bool isImguiInit;
		bool showMenu;
	} state;

	bool init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	bool loop(void(*renderCallback)());
	bool cleanup();

	static LRESULT WINAPI WndProcHook(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static bool handleImGuiInput(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LONG_PTR originalWndProc;
	static HWND window;

	bool setHookWndProc(HWND handleWindow, LONG_PTR wndProc = (LONG_PTR)WndProcHook);
	bool setWndProcToOriginal();

};

extern Render render;