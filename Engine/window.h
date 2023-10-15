#include "pch.h"

#ifndef WINDOW_H
#define WINDOW_H

#include "windows.h"

/*A window, it manages everything window related like, width, height, and when to close. Meant to be used as a pointer.*/
struct __declspec(dllexport) Window {
	HWND hwnd;
	bool shouldClose = false;

	const char* title;

	unsigned char* backBuffer = nullptr;
	int width;
	int height;

	unsigned int clearColor = 0x000000;

	void(*framebufferSizeCallback)(int, int) = nullptr;

	int scrollDelta = 0;
};

/*Creates a window with the specified parameters. Returns a window instance pointer. */
__declspec(dllexport) Window* InitializeWindow(int width, int height, const char* title);

/*Checks for events of the specified window.*/
__declspec(dllexport) void PollEvents(Window* window);

/*Checks if the specified window is set to close. Returns a boolean.*/
__declspec(dllexport) bool WindowShouldClose(Window* window);

/*Deletes the window from memory.*/
__declspec(dllexport) void DeleteWindow(Window* window);

/*Sets a callback that gets called on every resize event of the specified window.*/
__declspec(dllexport) void SetFramebufferSizeCallback(Window* window, void(*callback)(int, int));

/*Sets the title of the specified window.*/
__declspec(dllexport) void SetWindowTitle(Window* window, const char* title);

#endif