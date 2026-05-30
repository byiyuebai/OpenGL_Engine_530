#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
class LoadBlockData {
public:
	static void loadBlockDataFromFile(const std::string& filePath);
	static std::vector<glm::dvec2> uvOffset(glm::ivec2 tilePos, glm::dvec2 uvTileSize, glm::dvec2 uvPxSize);
private:

};