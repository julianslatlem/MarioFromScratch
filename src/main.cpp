//#include "engine.h"
//
//Window* window;
//
//int frameCount = 0;
//double frameTime = 0;
//
//double deltaTime = 0;
//static std::chrono::steady_clock::time_point lastTime;
//double timeAccumulator = 0;
//double updateInterval = 0.2f;
//
//int mouseX = 0;
//int mouseY = 0;
//
//void RenderScene();
//
//struct Camera {
//	int x;
//	int y;
//
//	int width;
//	int height;
//
//	float zoom;
//};
//
//struct StaticTile {
//	int ID;
//
//	const char* name;
//	Texture* texture;
//
//	bool collision;
//};
//
//StaticTile* CreateStaticTile(int ID, const char* name, Texture* texture, bool collision) {
//	StaticTile* tile = new StaticTile{};
//
//	tile->ID = ID;
//	tile->name = name;
//	tile->texture = texture;
//	tile->collision = collision;
//
//	return tile;
//}
//
//int level1_1[256][15] = {};
//
//StaticTile* staticTiles[4];
//
//StaticTile* GetStaticTileWithID(int targetID) {
//	for (int i = 0; i < sizeof(staticTiles) / sizeof(staticTiles[0]); i++) {
//		if (staticTiles[i]->ID == targetID) {
//			return staticTiles[i];
//		}
//	}
//
//	return nullptr;
//}
//
//void UpdateAverageFPS() {
//    auto currentTime = std::chrono::high_resolution_clock::now();
//    static auto lastTime = currentTime;
//    frameTime = std::chrono::duration<double>(currentTime - lastTime).count();
//    lastTime = currentTime;
//
//    if (frameTime > 0.9) {
//        frameTime = 0.9;
//    }
//
//    timeAccumulator += frameTime;
//    frameCount++;
//
//    double interval = 1.0;
//
//    if (timeAccumulator >= interval) {
//        double averageFPS = frameCount / timeAccumulator;
//
//        std::string windowTitle = "FPS: " + std::to_string(static_cast<int>(averageFPS));
//        SetWindowTitle(window, windowTitle.c_str());
//
//        timeAccumulator = 0.0;
//        frameCount = 0;
//    }
//}
//
//void UpdateDeltaTime() {
//	auto currentTime = std::chrono::high_resolution_clock::now();
//	deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();
//	lastTime = currentTime;
//}
//
//Camera* CreateCamera(int width, int height, float zoom) {
//	Camera* camera = new Camera{};
//
//	camera->x = 0;
//	camera->y = 0;
//	camera->width = width;
//	camera->height = height;
//	camera->zoom = zoom;
//
//	return camera;
//}
//
//Camera* camera;
//
//int main() {
//	window = InitializeWindow(1440, 1280, "Mario from Scratch");
//
//	camera = CreateCamera(1, 1, 1.0f);
//
//	Texture* groundTexture = CreateTexture("textures/ground.png");
//	Texture* brickTexture = CreateTexture("textures/brick.png");
//	Texture* c_brickTexture = CreateTexture("textures/brick_connected.png");
//	Texture* u_questionTexture = CreateTexture("textures/question_used.png");
//
//	staticTiles[0] = CreateStaticTile(1, "Ground", groundTexture, true);
//	staticTiles[1] = CreateStaticTile(2, "Brick", brickTexture, true);
//	staticTiles[2] = CreateStaticTile(3, "Connected Brick", c_brickTexture, true);
//	staticTiles[3] = CreateStaticTile(4, "Questionmark Block Used", u_questionTexture, true);
//
//	ClearColor(window, 0x9494ff);
//
//	while (!WindowShouldClose(window)) {
//		PollEvents(window);
//
//		UpdateDeltaTime();
//
//		Input::GetWindowMousePosition(window, mouseX, mouseY);
//
//		if (Input::GetKeyDown(KeyCode::RIGHT)) camera->x -= 500 * deltaTime;
//		if (Input::GetKeyDown(KeyCode::LEFT)) camera->x += 500 * deltaTime;
//		if (Input::GetKeyDown(KeyCode::UP)) camera->y += 500 * deltaTime;
//		if (Input::GetKeyDown(KeyCode::DOWN)) camera->y -= 500 * deltaTime;
//
//		if (Input::GetKeyDown(KeyCode::LEFT_MOUSE_BUTTON)) {
//			int x = (mouseX - camera->x) / 100;
//			int y = (mouseY - camera->y) / 100;
//			if (x <= 255 && x >= 0 && y <= 15 && y >= 0) level1_1[x][y] = 2;
//		}
//
//		UpdateAverageFPS();
//		RenderScene();
//	}
//}
//
//void RenderScene() {
//	Clear(window);
//
//	for (int y = 0; y <= 14; y++) {
//		for (int x = 0; x <= 255; x++) {
//			if (level1_1[x][y] != 0) {
//				Primitives::DrawTexture(window, camera->x + (x * 100), camera->y + (y * 100), 100, 100, GetStaticTileWithID(level1_1[x][y])->texture);
//			}
//		}
//	}
//
//	SwapBuffers(window);
//}