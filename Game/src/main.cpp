#include "engine.h"

#include "load.h"

Window* window;

double deltaTime = 0;
std::chrono::steady_clock::time_point lastTime;

double timeSinceLastUpdate = 0;

int level1_1[256][15] = {};
int level1_2[256][15] = {};
int level1_3[256][15] = {};

float tileSize = 100;

float timeLeft = 375;

void RenderScene();

void FramebufferSizeCallback(int width, int height) {
	RenderScene();
}

void UpdateDeltaTime() {
	auto currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();
	lastTime = currentTime;
}

Texture* coinUI = new Texture{};

int main() {
	window = InitializeWindow(1280, 720, "Super Mario Bros.");

	SetWindowIcon(window, "D:\\Programs\\VSStudio\\MarioFromScratch\\Build\\marioIcon.ico");

	ClearColor(window, 0x9494ff);

	SetFramebufferSizeCallback(window, FramebufferSizeCallback);

	CreateLevelFromFile("./levels/level1-1.ml", level1_1);
	CreateLevelFromFile("./levels/level1-2.ml", level1_2);
	CreateLevelFromFile("./levels/level1-3.ml", level1_3);

	Texture* coinUI1 = CreateTexture("./textures/coinUI.png");
	Texture* coinUI2 = CreateTexture("./textures/coinUI2.png");
	Texture* coinUI3 = CreateTexture("./textures/coinUI3.png");

	Texture* coinUItextures[4] = { coinUI1, coinUI1, coinUI2, coinUI3 };
	int currentCoinUItexture = 0;

	coinUI = coinUI1;

	PlayAudio("D:\\Programs\\VSStudio\\MarioFromScratch\\sound\\overworld.wav", 0);

	while (!WindowShouldClose(window)) {
		UpdateDeltaTime();
		PollEvents(window);
		
		timeSinceLastUpdate += deltaTime;

		if (timeSinceLastUpdate > 0.17f) {
			currentCoinUItexture++;
			if (currentCoinUItexture > 3) currentCoinUItexture = 0;

			coinUI = coinUItextures[currentCoinUItexture];

			timeLeft -= 0.5f;

			timeSinceLastUpdate = 0;
		}

		if (Input::GetKeyPressed(KeyCode::SPACE)) {
			PlayAudio("D:\\Programs\\VSStudio\\MarioFromScratch\\sound\\jump.wav", 1);
		}

		if (Input::GetKeyPressed(KeyCode::RIGHT_MOUSE_BUTTON)) {
			PlayAudio("D:\\Programs\\VSStudio\\MarioFromScratch\\sound\\shrink.wav", 2);
		}

		RenderScene();
	}

	DeleteWindow(window);
}

void RenderScene() {
	Clear(window);

	int fontSize = 5;

	Primitives::RenderText(window, 100, 100, "Mario", fontSize);
	Primitives::RenderText(window, 100, 100 + (8 * fontSize), "000000", fontSize);

	std::string timeLeftString = std::to_string((int)timeLeft);

	Primitives::RenderText(window, window->width - (100 + (4 * 8 * fontSize)), 100, "Time", fontSize);
	Primitives::RenderText(window, window->width - (100 + (3 * 8 * fontSize)), 100 + (8 * fontSize), timeLeftString.c_str(), fontSize);

	Primitives::RenderText(window, (window->width / 2) + (3 * 8 * fontSize), 100, "World", fontSize);
	Primitives::RenderText(window, (window->width / 2) + (4 * 8 * fontSize), 100 + (8 * fontSize), "1-1", fontSize);

	Primitives::RenderText(window, 100 + window->width / 4, 100 + (8 * fontSize), " *00", fontSize);

	Primitives::DrawTexture(window, 100 + window->width / 4, 100 + (8 * fontSize), 5 * fontSize, 8 * fontSize, coinUI);

	SwapBuffers(window);
}