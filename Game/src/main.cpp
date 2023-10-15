#include "engine.h"

Window* window;

void RenderScene();

void FramebufferSizeCallback(int width, int height) {
	RenderScene();
}

int main() {
	window = InitializeWindow(1280, 720, "Super Mario Bros.");

	ClearColor(window, 0x9494ff);

	SetFramebufferSizeCallback(window, FramebufferSizeCallback);

	while (!WindowShouldClose(window)) {
		PollEvents(window);

		RenderScene();
	}
}

void RenderScene() {
	Clear(window);

	SwapBuffers(window);
}