#ifndef RENDERER_H
#define RENDERER_H

#include "windows.h"
#include "window.h"

/*A struct that includes things like a width and height, and texture data. Meant to be used as a pointer.*/
struct Texture {
	int desiredComponents = 4;

	int width;
	int height;

	int components;
	unsigned char* data;
};

/*Sets the specified pixel to the specified color.*/
void SetPixel(Window* window, int x, int y, unsigned int color);

/*Gets the specified pixel's color. Returns a color.*/
unsigned int GetPixel(Window* window, int x, int y);

/*Sets the current clear color.*/
void ClearColor(Window* window, unsigned int color);

/*Clears the entire back buffer with the currently set color.*/
void Clear(Window* window);

/*Swaps the back and front buffers of the window, effectively rendering whats set to render.*/
void SwapBuffers(Window* window);

/*Creates a texture. Returns a pointer to a texture.*/
Texture* CreateTexture(const char* filepath);

/*Deletes the specified texture from memory.*/
void DeleteTexture(Texture* texture);

/*A collection of simple primitives.*/
namespace Primitives {
	/*Draws a simple rect to the back buffer.*/
	void DrawRect(Window* window, int x, int y, int width, int height, unsigned int color);

	/*Draws a simple circle to the back buffer.*/
	void DrawCircle(Window* window, int x, int y, int radius, unsigned int color, bool antiAliasing = false);

	/*Draws a simple anti-aliased line between two points to the back buffer.*/
	void DrawLine(Window* window, int x0, int y0, int x1, int y1, unsigned int color, bool antiAliasing = false);

	/*Draws a simple triangle to the back buffer.*/
	void DrawTriangle(Window* window, int t0X, int t0Y, int t1X, int t1Y, int t2X, int t2Y, unsigned int color, bool antiAliasing = false);

	/*Draws a simple rectangle with a texture applied.*/
	void DrawTexture(Window* window, int x, int y, int width, int height, Texture* texture);
}

#endif