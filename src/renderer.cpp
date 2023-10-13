#include "renderer.h"
#include "color.h"
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void SetPixel(Window* window, int x, int y, unsigned int color) {
	unsigned char r = color & 0xFF;
	unsigned char g = (color >> 8) & 0xFF;
	unsigned char b = (color >> 16) & 0xFF;
	unsigned char a = (color >> 24) & 0xFF;

	if (x >= 0 && x < window->width && y >= 0 && y < window->height) {
		unsigned char* pixel = window->backBuffer + ((x + y * window->width) * 4);
		pixel[0] = r;
		pixel[1] = g;
		pixel[2] = b;
		pixel[3] = a;
	}
}

unsigned int GetPixel(Window* window, int x, int y) {
	if (x >= 0 && x < window->width && y >= 0 && y < window->height) {
		unsigned char* pixel = window->backBuffer + ((x + y * window->width) * 4);
		unsigned char r = pixel[0];
		unsigned char g = pixel[1];
		unsigned char b = pixel[2];
		unsigned char a = pixel[3];
		return  r | (g << 8) | (b << 16) | (a << 24);
	}

	return 0;
}

void ClearColor(Window* window, unsigned int color) {
	window->clearColor = color;
}

void Clear(Window* window) {
	unsigned char* pixels = window->backBuffer;

	unsigned char r = window->clearColor & 0xFF;
	unsigned char g = (window->clearColor >> 8) & 0xFF;
	unsigned char b = (window->clearColor >> 16) & 0xFF;
	unsigned char a = (window->clearColor >> 24) & 0xFF;


	for (int i = 0; i < window->width * window->height; ++i) {
		pixels[i * 4] = r;
		pixels[i * 4 + 1] = g;
		pixels[i * 4 + 2] = b;
		pixels[i * 4 + 3] = a;
	}
}

void SwapBuffers(Window* window) {
	HDC hdc = GetDC(window->hwnd);

	HDC backBufferDC = CreateCompatibleDC(hdc);

	HBITMAP hBackBuffer = CreateBitmap(window->width, window->height, 1, 32, window->backBuffer);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(backBufferDC, hBackBuffer);

	BitBlt(hdc, 0, 0, window->width, window->height, backBufferDC, 0, 0, SRCCOPY);

	SelectObject(backBufferDC, hOldBitmap);
	DeleteObject(hBackBuffer);
	DeleteDC(backBufferDC);

	ReleaseDC(window->hwnd, hdc);
}

Texture* CreateTexture(const char* filepath) {
	Texture* texture = new Texture{};

	stbi_image_free(texture->data);
	texture->data = stbi_load(filepath, &texture->width, &texture->height, &texture->components, texture->desiredComponents);

	return texture;
}

void DeleteTexture(Texture* texture) {
	delete texture;
}

namespace Primitives {
	#include "math.h"

	void DrawRect(Window* window, int x, int y, int width, int height, unsigned int color) {
		for (int yy = y; yy <= y + height; yy++) {
			for (int xx = x; xx <= x + width; xx++) {
				SetPixel(window, xx, yy, color);
			}
		}
	}

	void DrawCircle(Window* window, int x, int y, int radius, unsigned int color, bool antiAlising) {
		if (antiAlising) {
			for (int yy = -radius; yy <= radius; yy++) {
				for (int xx = -radius; xx <= radius; xx++) {
					double distance = sqrt(xx * xx + yy * yy);

					if (distance <= radius) {
						double alpha = radius * (1.0f - (distance / radius));

						alpha = max(0.0f, min(1.0f, alpha));

						if (alpha > 0.0f) {
							unsigned int blendedColor = BlendRGBA(color, GetPixel(window, xx + x, yy + y), alpha);
							SetPixel(window, xx + x, yy + y, blendedColor);
						}
					}
				}
			}
		}
		else {
			for (int yy = -radius; yy <= radius; yy++) {
				for (int xx = -radius; xx <= radius; xx++) {
					double distance = sqrt(xx * xx + yy * yy);

					if (distance <= radius) {
						SetPixel(window, xx + x, yy + y, color);
					}
				}
			}
		}
	}

	void DrawLine(Window* window, int x0, int y0, int x1, int y1, unsigned int color, bool antiAliasing) {
		if (antiAliasing) {
			bool steep = abs(y1 - y0) > abs(x1 - x0);

			if (steep) {
				swap(x0, y0);
				swap(x1, y1);
			}

			if (x0 > x1) {
				swap(x0, x1);
				swap(y0, y1);
			}

			int dx = x1 - x0;
			int dy = y1 - y0;
			float gradient = static_cast<float>(dy) / dx;

			int xpxl1 = x0;
			int xpxl2 = x1;
			float intery = y0;

			float alpha = 1.0f;

			for (int x = xpxl1; x <= xpxl2; x++) {
				if (steep) {
					int y = static_cast<int>(intery);
					alpha = intery - static_cast<int>(intery);
					SetPixel(window, y, x, BlendRGBA(GetPixel(window, y, x), color, alpha));
					SetPixel(window, y + 1, x, BlendRGBA(GetPixel(window, y + 1, x), color, 1.0f - alpha));
				}
				else {
					int y = static_cast<int>(intery);
					alpha = intery - static_cast<int>(intery);
					SetPixel(window, x, y, BlendRGBA(GetPixel(window, x, y), color, alpha));
					SetPixel(window, x, y + 1, BlendRGBA(GetPixel(window, x, y + 1), color, 1.0f - alpha));
				}
				intery += gradient;
			}
		}
		else {
			int dx = abs(x1 - x0);
			int dy = -abs(y1 - y0);
			int sx = x0 < x1 ? 1 : -1;
			int sy = y0 < y1 ? 1 : -1;
			int err = dx + dy;

			while (true) {
				SetPixel(window, x0, y0, BlendRGBA(GetPixel(window, x0, y0), color, 0.8f));

				if (x0 == x1 && y0 == y1) {
					break;
				}

				int e2 = 2 * err;

				if (e2 >= dy) {
					err += dy;
					x0 += sx;
				}

				if (e2 <= dx) {
					err += dx;
					y0 += sy;
				}
			}
		}
	}

	void DrawTriangle(Window* window, int t0X, int t0Y, int t1X, int t1Y, int t2X, int t2Y, unsigned int color, bool antiAliasing) {
		if (t0Y == t1Y && t0Y == t2Y) return;

		if (t0Y > t1Y) {
			swap(t0X, t1X);
			swap(t0Y, t1Y);
		}
		if (t0Y > t2Y) {
			swap(t0X, t2X);
			swap(t0Y, t2Y);
		}
		if (t1Y > t2Y) {
			swap(t1X, t2X);
			swap(t1Y, t2Y);
		}

		int total_height = t2Y - t0Y;
		for (int i = 0; i < total_height; i++) {
			bool second_half = i > t1Y - t0Y || t1Y == t0Y;
			int segment_height = second_half ? t2Y - t1Y : t1Y - t0Y;
			float alpha = (float)i / total_height;
			float beta = (float)(i - (second_half ? t1Y - t0Y : 0)) / segment_height;

			int Ax = t0X + (t2X - t0X) * alpha;
			int Ay = t0Y + (t2Y - t0Y) * alpha;
			int Bx = second_half ? t1X + (t2X - t1X) * beta : t0X + (t1X - t0X) * beta;
			int By = second_half ? t1Y + (t2Y - t1Y) * beta : t0Y + (t1Y - t0Y) * beta;

			if (Ax > Bx) {
				swap(Ax, Bx);
				swap(Ay, By);
			}

			for (int j = Ax; j <= Bx; j++) {
				SetPixel(window, j, t0Y + i, color);
			}
		}

		if (antiAliasing) {
			DrawLine(window, t0X, t0Y, t1X, t1Y, color, true);
			DrawLine(window, t1X, t1Y, t2X, t2Y, color, true);
			DrawLine(window, t2X, t2Y, t0X, t0Y, color, true);
		}
	}

	void DrawTexture(Window* window, int x, int y, int width, int height, Texture* texture) {
		if (!texture || !texture->data || !window->backBuffer) {
			return;
		}

		float scaleX = static_cast<float>(width) / static_cast<float>(texture->width);
		float scaleY = static_cast<float>(height) / static_cast<float>(texture->height);

		unsigned char* destBuffer = window->backBuffer;

		for (int dy = max(0, -y); dy < min(height, window->height - y); dy++) {
			for (int dx = max(0, -x); dx < min(width, window->width - x); dx++) {
				int srcX = static_cast<int>(static_cast<float>(dx) / scaleX);
				int srcY = static_cast<int>(static_cast<float>(dy) / scaleY);

				if (srcX < 0 || srcX >= texture->width || srcY < 0 || srcY >= texture->height) {
					// Skip pixels that are outside the texture boundaries
					continue;
				}

				int textureIndex = (srcY * texture->width + srcX) * texture->components;

				unsigned char r = texture->data[textureIndex + 2];
				unsigned char g = texture->data[textureIndex + 1];
				unsigned char b = texture->data[textureIndex + 0];
				unsigned char a = texture->data[textureIndex + 3];

				int destIndex = (y + dy) * window->width + (x + dx);

				if (a == 0xFF) {
					// If the source pixel is fully opaque, directly overwrite the destination pixel
					destBuffer[destIndex * 4] = r;
					destBuffer[destIndex * 4 + 1] = g;
					destBuffer[destIndex * 4 + 2] = b;
					destBuffer[destIndex * 4 + 3] = a;
				}
				else if (a != 0) {
					// If the source pixel has some transparency, blend it with the destination pixel
					unsigned int destColor = (destBuffer[destIndex * 4 + 3] << 24) | (destBuffer[destIndex * 4] << 16) | (destBuffer[destIndex * 4 + 1] << 8) | destBuffer[destIndex * 4 + 2];
					unsigned int srcColor = (a << 24) | (r << 16) | (g << 8) | b;
					unsigned int blendedColor = BlendRGBA(destColor, srcColor, 0.5f); // You need to implement the BlendColors function

					destBuffer[destIndex * 4] = (blendedColor >> 16) & 0xFF;
					destBuffer[destIndex * 4 + 1] = (blendedColor >> 8) & 0xFF;
					destBuffer[destIndex * 4 + 2] = blendedColor & 0xFF;
					destBuffer[destIndex * 4 + 3] = (blendedColor >> 24) & 0xFF;
				}
			}
		}
	}
}