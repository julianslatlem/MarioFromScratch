#include "engine.h"
#include <fstream>
#include <shobjidl.h>
#include <locale>
#include <codecvt>

Window* window;

int frameCount = 0;
double frameTime = 0;

double deltaTime = 0;
static std::chrono::steady_clock::time_point lastTime;
double timeAccumulator = 0;
double updateInterval = 0.2f;

int mouseX = 0;
int mouseY = 0;
int lastX = 0;
int lastY = 0;

int tileSize = 100;
int activeTileID = 1;

int currentQuestionTexture = 0;

float timeSinceLastUpdate = 0;

std::string installDir;

std::string levelFile = "levels/level1-1.ml";

void RenderScene();

struct Camera {
	int x;
	int y;

	int width;
	int height;

	float zoom;
};

struct StaticTile {
	int ID;

	const char* name;
	Texture* texture;

	bool collision;
};

StaticTile* CreateStaticTile(int ID, const char* name, Texture* texture, bool collision) {
	StaticTile* tile = new StaticTile{};

	tile->ID = ID;
	tile->name = name;
	tile->texture = texture;
	tile->collision = collision;

	return tile;
}

int level1_1[256][15] = {};

StaticTile* staticTiles[15];

StaticTile* GetStaticTileWithID(int targetID) {
	for (int i = 0; i < sizeof(staticTiles) / sizeof(staticTiles[0]); i++) {
		if (staticTiles[i]->ID == targetID) {
			return staticTiles[i];
		}
	}

	return nullptr;
}

void UpdateAverageFPS() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    static auto lastTime = currentTime;
    frameTime = std::chrono::duration<double>(currentTime - lastTime).count();
    lastTime = currentTime;

    if (frameTime > 0.9) {
        frameTime = 0.9;
    }

    timeAccumulator += frameTime;
    frameCount++;

    double interval = 1.0;

    if (timeAccumulator >= interval) {
        double averageFPS = frameCount / timeAccumulator;

        std::string windowTitle = "FPS: " + std::to_string(static_cast<int>(averageFPS));
        SetWindowTitle(window, windowTitle.c_str());

        timeAccumulator = 0.0;
        frameCount = 0;
    }
}

void UpdateDeltaTime() {
	auto currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();
	lastTime = currentTime;
}

Camera* CreateCamera(int width, int height, float zoom) {
	Camera* camera = new Camera{};

	camera->x = 0;
	camera->y = 0;
	camera->width = width;
	camera->height = height;
	camera->zoom = zoom;

	return camera;
}

Camera* camera;

bool LoadLevel(std::string filePath) {
	std::ifstream inFile(filePath);

	if (inFile.is_open()) {
		for (int i = 0; i < 256; i++) {
			for (int j = 0; j < 15; j++) {
				std::string value;
				if (!(inFile >> value)) {
					std::cerr << "Error: Failed to read data from the file. 1" << std::endl;
					inFile.close();
					return false;
				}

				level1_1[i][j] = std::stoi(value);
			}
		}

		std::cout << "Loaded File: " << filePath << std::endl;

		inFile.close();
		return true;
	}
	else {
		std::cerr << "Error: Unable to open the file for reading." << std::endl;
	}

	inFile.close();
	return false;
}

bool SaveLevel(PWSTR filePath) {
	std::ofstream outFile(filePath);

	if (outFile.is_open()) {
		for (int i = 0; i < 256; i++) {
			for (int j = 0; j < 15; j++) {
				outFile << level1_1[i][j] << ' ';
			}
			outFile << '\n';
		}

		outFile.close();
		return true;
	}
	else {
		std::cerr << "Error: Unable to open the file for writing." << std::endl;
	}

	outFile.close();
	return false;
}

std::string CombinePaths(const std::string& path1, const std::string& path2) {
	char separator =
#ifdef _WIN32
		'\\'; // Windows path separator
#else
		'/'; // Other platforms use forward slash
#endif

	if (!path1.empty() && path1.back() != separator && path2.front() != separator) {
		return path1 + separator + path2;
	}
	else if (path1.empty() || path2.empty()) {
		return path1 + path2;
	}
	else {
		return path1 + path2;
	}
}

int main(int argc, char* argv[]) {
	wchar_t buffer[MAX_PATH];
	if (GetModuleFileName(NULL, buffer, MAX_PATH) != 0) {
		std::wstring executablePath = buffer;
		size_t lastSeparator = executablePath.find_last_of(L"\\/");
		if (lastSeparator != std::wstring::npos) {
			std::wstring installDirectory = executablePath.substr(0, lastSeparator);
			// Convert the Wide Character string to a regular string
			std::string narrowInstallDirectory(installDirectory.begin(), installDirectory.end());
			//std::cout << "Install directory: " << narrowInstallDirectory << std::endl;

			installDir = narrowInstallDirectory;
		}
		else {
			std::cerr << "Failed to extract the install directory." << std::endl;
		}
	}
	else {
		std::cerr << "Failed to get the module file name." << std::endl;
	}

	if (argc > 1) {
		const char* narrowFilePath = argv[1];
		int bufferSize = MultiByteToWideChar(CP_UTF8, 0, narrowFilePath, -1, nullptr, 0);

		if (bufferSize > 0) {
			wchar_t* wideFilePath = new wchar_t[bufferSize];
			MultiByteToWideChar(CP_UTF8, 0, narrowFilePath, -1, wideFilePath, bufferSize);

			// Now you have a wide character string
			std::wcout << L"Converted File Path: " << wideFilePath << std::endl;

			// Convert wide character string to UTF-8
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			std::string utf8FilePath = converter.to_bytes(wideFilePath);

			std::cout << "Converted File Path (UTF-8): " << utf8FilePath << std::endl;

			// Verify that `levelFile` is correctly declared and defined
			levelFile = utf8FilePath;

			for (size_t i = 0; i < levelFile.size(); ++i) {
				if (levelFile[i] == '\\') {
					levelFile[i] = '/';
				}
			}

			std::cout << "Normalized Level File Path: " << levelFile << std::endl;

			// Add debugging output in the LoadLevel function
			if (LoadLevel(levelFile)) {
				std::cout << "File loaded successfully." << std::endl;
			}
			else {
				std::cerr << "Failed to load the file." << std::endl;
			}

			delete[] wideFilePath;
		}
		else {
			std::cerr << "Failed to convert wide character string to multibyte." << std::endl;
		}
	}









	window = InitializeWindow(1440, 1275, "Mario from Scratch");

	camera = CreateCamera(1, 1, 1.0f);

	Texture* groundTexture = CreateTexture(CombinePaths(installDir, "textures/ground.png").c_str());
	Texture* brickTexture = CreateTexture(CombinePaths(installDir, "textures/brick.png").c_str());
	Texture* c_brickTexture = CreateTexture(CombinePaths(installDir, "textures/brick_connected.png").c_str());
	Texture* questionTexture = CreateTexture(CombinePaths(installDir, "textures/question.png").c_str());
	Texture* questionTexture2 = CreateTexture(CombinePaths(installDir, "textures/question2.png").c_str());
	Texture* questionTexture3 = CreateTexture(CombinePaths(installDir, "textures/question3.png").c_str());
	Texture* u_questionTexture = CreateTexture(CombinePaths(installDir, "textures/question_used.png").c_str());
	Texture* bush1_1 = CreateTexture(CombinePaths(installDir, "textures/bush1-1.png").c_str());
	Texture* bush1_2 = CreateTexture(CombinePaths(installDir, "textures/bush1-2.png").c_str());
	Texture* bush1_3 = CreateTexture(CombinePaths(installDir, "textures/bush1-3.png").c_str());
	Texture* bush1_4 = CreateTexture(CombinePaths(installDir, "textures/bush1-4.png").c_str());
	Texture* bush1_5 = CreateTexture(CombinePaths(installDir, "textures/bush1-5.png").c_str());
	Texture* bush1_6 = CreateTexture(CombinePaths(installDir, "textures/bush1-6.png").c_str());
	Texture* bush2_1 = CreateTexture(CombinePaths(installDir, "textures/bush2-1.png").c_str());
	Texture* bush2_2 = CreateTexture(CombinePaths(installDir, "textures/bush2-2.png").c_str());
	Texture* bush2_3 = CreateTexture(CombinePaths(installDir, "textures/bush2-3.png").c_str());

	Texture* questionTextures[4] = { questionTexture, questionTexture, questionTexture2, questionTexture3 };

	staticTiles[0] = CreateStaticTile(0, "Air", new Texture{}, false);
	staticTiles[1] = CreateStaticTile(1, "Ground", groundTexture, true);
	staticTiles[2] = CreateStaticTile(2, "Brick", brickTexture, true);
	staticTiles[3] = CreateStaticTile(3, "Connected Brick", c_brickTexture, true);
	staticTiles[4] = CreateStaticTile(4, "Questionmark Block", questionTexture, true);
	staticTiles[5] = CreateStaticTile(5, "Questionmark Block Used", u_questionTexture, true);
	staticTiles[6] = CreateStaticTile(6, "Bush1-1", bush1_1, false);
	staticTiles[7] = CreateStaticTile(7, "Bush1-2", bush1_2, false);
	staticTiles[8] = CreateStaticTile(8, "Bush1-3", bush1_3, false);
	staticTiles[9] = CreateStaticTile(9, "Bush1-4", bush1_4, false);
	staticTiles[10] = CreateStaticTile(10, "Bush1-5", bush1_5, false);
	staticTiles[11] = CreateStaticTile(11, "Bush1-6", bush1_6, false);
	staticTiles[12] = CreateStaticTile(12, "Bush2-1", bush2_1, false);
	staticTiles[13] = CreateStaticTile(13, "Bush2-2", bush2_2, false);
	staticTiles[14] = CreateStaticTile(14, "Bush2-3", bush2_3, false);

	

	ClearColor(window, 0x9494ff);

	while (!WindowShouldClose(window)) {
		PollEvents(window);

		GetStaticTileWithID(4)->texture = questionTextures[currentQuestionTexture];

		timeSinceLastUpdate += deltaTime;

		if (timeSinceLastUpdate > 0.2f) {
			currentQuestionTexture++;
			if (currentQuestionTexture > 3) currentQuestionTexture = 0;

			timeSinceLastUpdate = 0;
		}

		UpdateDeltaTime();

		tileSize = window->height / 15;

		Input::GetWindowMousePosition(window, mouseX, mouseY);

		if (Input::GetKeyPressed(KeyCode::RIGHT)) activeTileID++;
		if (Input::GetKeyPressed(KeyCode::LEFT)) activeTileID--;

		int scrollDelta = Input::GetScrollDelta(window) / 120;

		activeTileID += scrollDelta;

		if (activeTileID < 0) activeTileID = 14;
		if (activeTileID > 14) activeTileID = 0;

		if (Input::GetKeyPressed(KeyCode::MIDDLE_MOUSE_BUTTON)) {
			lastX = mouseX;
			lastY = mouseY;
		}
		if (Input::GetKeyDown(KeyCode::MIDDLE_MOUSE_BUTTON)) {
			int deltaX = mouseX - lastX;
			int deltaY = mouseY - lastY;
			camera->x += deltaX;
			camera->y += deltaY;

			lastX = mouseX;
			lastY = mouseY;
		}

		if (Input::GetKeyDown(KeyCode::LEFT_MOUSE_BUTTON)) {
			int x = (mouseX - camera->x) / tileSize;
			int y = (mouseY - camera->y) / tileSize;
			if (x <= 255 && x >= 0 && y <= 14 && y >= 0 && mouseX > camera->x && mouseY > camera->y) level1_1[x][y] = activeTileID;
		}

		if (Input::GetKeyDown(KeyCode::CTRL) && Input::GetKeyPressed(KeyCode::S)) {
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr)) {
				IFileSaveDialog* pFileSave;

				// Create the File Save dialog
				hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

				if (SUCCEEDED(hr)) {
					// Show the dialog
					hr = pFileSave->Show(NULL);

					if (SUCCEEDED(hr)) {
						IShellItem* pItem;
						hr = pFileSave->GetResult(&pItem);

						if (SUCCEEDED(hr)) {
							PWSTR filePath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);

							if (SUCCEEDED(hr)) {
								SaveLevel(filePath);

								CoTaskMemFree(filePath);
							}
							pItem->Release();
						}
					}
					pFileSave->Release();
				}
				CoUninitialize();
			}
		}
		if (Input::GetKeyDown(KeyCode::CTRL) && Input::GetKeyPressed(KeyCode::L)) {
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr)) {
				IFileOpenDialog* pFileOpen;

				// Create the File Open dialog
				hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

				if (SUCCEEDED(hr)) {
					// Show the dialog
					hr = pFileOpen->Show(NULL);

					if (SUCCEEDED(hr)) {
						IShellItem* pItem;
						hr = pFileOpen->GetResult(&pItem);

						if (SUCCEEDED(hr)) {
							PWSTR wideFilePath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &wideFilePath);

							if (SUCCEEDED(hr)) {
								// Convert the wide character path to a narrow character path
								int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideFilePath, -1, NULL, 0, NULL, NULL);

								if (bufferSize > 0) {
									char* narrowFilePath = new char[bufferSize];
									WideCharToMultiByte(CP_UTF8, 0, wideFilePath, -1, narrowFilePath, bufferSize, NULL, NULL);

									std::string filePath = narrowFilePath;

									// Load the file using filePath
									LoadLevel(filePath);

									CoTaskMemFree(wideFilePath);
									pItem->Release();
									pFileOpen->Release();
									CoUninitialize();
								}
								else {
									std::cerr << "Failed to convert wide character string to multibyte." << std::endl;
								}
							}
							pItem->Release();
						}
					}
					pFileOpen->Release();
				}
				CoUninitialize();
			}
			else {
				std::cerr << "COM initialization failed." << std::endl;
			}
		}

		UpdateAverageFPS();
		RenderScene();
	}
}

void RenderScene() {
	Clear(window);

	for (int y = 0; y <= 14; y++) {
		for (int x = 0; x <= 255; x++) {
			if (level1_1[x][y] != 0) {
				Primitives::DrawTexture(window, camera->x + (x * tileSize), camera->y + (y * tileSize), tileSize, tileSize, GetStaticTileWithID(level1_1[x][y])->texture);
			}
		}
	}

	Texture* activeTexture = GetStaticTileWithID(activeTileID)->texture;

	Texture* newTexture = new Texture{};
	newTexture->components = activeTexture->components;
	newTexture->desiredComponents = activeTexture->desiredComponents;
	newTexture->height = activeTexture->height;
	newTexture->width = activeTexture->width;

	newTexture->data = new unsigned char[newTexture->width * newTexture->height * newTexture->components];

	std::copy(activeTexture->data, activeTexture->data + newTexture->width * newTexture->height * newTexture->components, newTexture->data);

	for (int i = 0; i < newTexture->width * newTexture->height; i++) {
		newTexture->data[i * newTexture->components + 3] *= 0.5f;
	}

	int x = (mouseX - camera->x) / tileSize;
	int y = (mouseY - camera->y) / tileSize;

	if (x <= 255 && x >= 0 && y <= 14 && y >= 0 && mouseX > camera->x && mouseY > camera->y) Primitives::DrawTexture(window, (camera->x + ((mouseX - camera->x) / tileSize) * tileSize), camera->y + (((mouseY - camera->y) / tileSize) * tileSize), tileSize, tileSize, newTexture);

	Primitives::DrawLine(window, camera->x, 0, camera->x, window->height, 0xffffff);
	Primitives::DrawLine(window, camera->x - 1, 0, camera->x - 1, window->height, 0xffffff);
	Primitives::DrawLine(window, camera->x + (tileSize * 256), 0, camera->x + (tileSize * 256), window->height, 0xffffff);
	Primitives::DrawLine(window, camera->x + (tileSize * 256) + 1, 0, camera->x + (tileSize * 256) + 1, window->height, 0xffffff);
	Primitives::DrawLine(window, 0, camera->y, window->width, camera->y, 0xffffff);
	Primitives::DrawLine(window, 0, camera->y - 1, window->width, camera->y - 1, 0xffffff);
	Primitives::DrawLine(window, 0, camera->y + (tileSize * 15), window->width, camera->y + (tileSize * 15), 0xffffff);
	Primitives::DrawLine(window, 0, camera->y + (tileSize * 15) + 1, window->width, camera->y + (tileSize * 15) + 1, 0xffffff);

	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize), 0, (camera->x + (-camera->x / tileSize) * tileSize), window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 2, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 2, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 3, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 3, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 4, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 4, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 5, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 5, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 6, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 6, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 7, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 7, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 8, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 8, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 9, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 9, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 10, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 10, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 11, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 11, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 12, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 12, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 13, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 13, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 14, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 14, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 15, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 15, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 16, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 16, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 17, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 17, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 18, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 18, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 19, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 19, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 20, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 20, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 21, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 21, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 22, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 22, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 23, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 23, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 24, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 24, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 25, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 25, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 26, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 26, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 27, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 27, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 28, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 28, window->height, 0xffffff);
	Primitives::DrawLine(window, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 29, 0, (camera->x + (-camera->x / tileSize) * tileSize) + tileSize * 29, window->height, 0xffffff);

	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize), window->width, (camera->y + (-camera->y / tileSize) * tileSize), 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 2, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 2, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 3, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 3, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 4, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 4, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 5, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 5, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 6, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 6, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 7, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 7, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 8, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 8, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 9, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 9, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 10, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 10, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 11, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 11, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 12, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 12, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 13, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 13, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 14, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 14, 0xffffff);
	Primitives::DrawLine(window, 0, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 15, window->width, (camera->y + (-camera->y / tileSize) * tileSize) + tileSize * 15, 0xffffff);

	DeleteTexture(newTexture);

	SwapBuffers(window);
}