#include "load.h"

#include <fstream>
#include <string>

bool CreateLevelFromFile(std::string filePath, int level[256][15]) {
	std::ifstream inFile(filePath);

	if (inFile.is_open()) {
		for (int i = 0; i < 256; i++) {
			for (int j = 0; j < 15; j++) {
				std::string value;
				if (!(inFile >> value)) {
					std::cerr << "Error: Failed to read data from the file: " << filePath << std::endl;
					inFile.close();
					return false;
				}

				level[i][j] = std::stoi(value);
			}
		}

		std::cout << "Created level from file: " << filePath << std::endl;

		inFile.close();
		return true;
	}
	else {
		std::cerr << "Error: Unable to open the file for reading: " << filePath << std::endl;
	}

	inFile.close();
	return false;
}