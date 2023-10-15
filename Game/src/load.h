#pragma once
#include <iostream>

/*Loads the specified level into the specified int array.*/
bool CreateLevelFromFile(std::string filePath, int level[256][15]);