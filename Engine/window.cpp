#include "pch.h"
#include <stdlib.h>

#include "window.h"
#include "input.h"

LRESULT CALLBACK WindowCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (msg) {
	case WM_CLOSE:
	case WM_DESTROY: {
		PostQuitMessage(0);
		if (window) window->shouldClose = true;
	} break;

	case WM_SIZE: {
		int newWidth = LOWORD(lParam);
		int newHeight = HIWORD(lParam);
		if (window) {
			window->width = newWidth;
			window->height = newHeight;

			free(window->backBuffer);
			size_t bufferSize = newWidth * newHeight * sizeof(unsigned char*);
			window->backBuffer = (unsigned char*)malloc(bufferSize);

			if (window->framebufferSizeCallback != nullptr) window->framebufferSizeCallback(newWidth, newHeight);
		}
	} break;

	case WM_MOUSEWHEEL: {
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);

		window->scrollDelta = delta;
	} break;

	default: {
		result = DefWindowProcA(hwnd, msg, wParam, lParam);
	}
	}

	return result;
}

Window* InitializeWindow(int width, int height, const char* title) {
	Window* window = new Window{};

	HINSTANCE instance = GetModuleHandleA(0);

	WNDCLASSA windowClass = {};
	windowClass.hInstance = instance;
	windowClass.hCursor = LoadCursor(0, IDC_ARROW);
	windowClass.lpszClassName = title;
	windowClass.lpfnWndProc = WindowCallback;

	if (!RegisterClassA(&windowClass)) {
		delete window; return nullptr;
	}

	int dwStyle = WS_OVERLAPPEDWINDOW;

	RECT windowRect = { 0, 0, width, height };
	AdjustWindowRectEx(&windowRect, dwStyle, false, 0);

	int adjustedWidth = windowRect.right - windowRect.left;
	int adjustedHeight = windowRect.bottom - windowRect.top;

	HWND hwnd = CreateWindowExA(0, title, title, dwStyle, 100, 100, adjustedWidth, adjustedHeight, 0, 0, instance, 0);
	if (!hwnd) {
		delete window; return nullptr;
	}

	ShowWindow(hwnd, SW_SHOW);

	window->hwnd = hwnd;
	window->width = width;
	window->height = height;

	int bufferSize = width * height * sizeof(unsigned char*);
	window->backBuffer = (unsigned char*)malloc(bufferSize);
	if (!window->backBuffer) {
		delete window;
		return nullptr;
	}

	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

	return window;
}

void PollEvents(Window* window) {
	MSG msg;

	while (PeekMessage(&msg, window->hwnd, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}

bool WindowShouldClose(Window* window) {
	return window->shouldClose;
}

void DeleteWindow(Window* window) {
	delete window;
}

void SetFramebufferSizeCallback(Window* window, void(*callback)(int, int)) {
	window->framebufferSizeCallback = callback;
}

void SetWindowTitle(Window* window, const char* title) {
	window->title = title;

	SetWindowTextA(window->hwnd, window->title);
}

void SetWindowIcon(Window* window, const char* iconFilePath) {
	HICON hIcon = (HICON)LoadImageA(NULL, iconFilePath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	if (hIcon) {
		SetClassLongPtr(window->hwnd, GCLP_HICON, (LONG_PTR)hIcon);
		DestroyIcon(hIcon);
	}
}